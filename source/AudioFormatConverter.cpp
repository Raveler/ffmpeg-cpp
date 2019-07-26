#include "AudioFormatConverter.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{
	AudioFormatConverter::AudioFormatConverter(ConvertedAudioProcessor* writer, AVCodecContext* codecContext)
	{
		this->output = writer;
		this->codecContext = codecContext;

		converted_frame = av_frame_alloc();
		int ret;
		if (!converted_frame)
		{
			CleanUp();
			throw FFmpegException("Error allocating an audio frame");
		}

		// calculate the sample count
		int nb_samples;
		if (codecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
			nb_samples = 10000;
		else
			nb_samples = codecContext->frame_size;

		// configure the frame based on our codec context
		converted_frame->format = codecContext->sample_fmt;
		converted_frame->channel_layout = codecContext->channel_layout;
		converted_frame->sample_rate = codecContext->sample_rate;
		converted_frame->nb_samples = nb_samples;
		if (nb_samples)
		{
			ret = av_frame_get_buffer(converted_frame, 0);
			if (ret < 0)
			{
				CleanUp();
				throw FFmpegException("Error allocating an audio buffer", ret);
			}
		}

		// create the temporary frame that will hold parts of the converted data
		// this data will later be assembled in a complete converted_frame.
		tmp_frame = av_frame_alloc();
		if (!tmp_frame)
		{
			CleanUp();
			throw FFmpegException("Error allocating an audio frame");
		}
		tmp_frame->format = codecContext->sample_fmt;
		tmp_frame->channel_layout = codecContext->channel_layout;
		tmp_frame->sample_rate = codecContext->sample_rate;
		tmp_frame->nb_samples = 0;

		// Create the FIFO buffer based on the specified output sample format
		fifo = av_audio_fifo_alloc(codecContext->sample_fmt, codecContext->channels, nb_samples);
		if (!fifo)
		{
			CleanUp();
			throw FFmpegException("Failed to create FIFO queue for audio format converter");
		}
	}

	AudioFormatConverter::~AudioFormatConverter()
	{
		CleanUp();
	}

	void AudioFormatConverter::CleanUp()
	{
		if (converted_frame != nullptr)
		{
			av_frame_free(&converted_frame);
			converted_frame = nullptr;
		}
		if (tmp_frame != nullptr)
		{
			av_frame_free(&tmp_frame);
			tmp_frame = nullptr;
		}
		if (swr_ctx != nullptr)
		{
			swr_free(&swr_ctx);
			swr_ctx = nullptr;
		}
		if (fifo != nullptr)
		{
			av_audio_fifo_free(fifo);
			fifo = nullptr;
		}
	}

	void AudioFormatConverter::InitDelayed(AVFrame* frame)
	{
		swr_ctx = swr_alloc();
		if (!swr_ctx)
		{
			throw FFmpegException("Could not allocate resampler context");
		}

		// set options
		in_sample_rate = frame->sample_rate;
		out_sample_rate = codecContext->sample_rate;
		av_opt_set_int(swr_ctx, "in_channel_count", frame->channels, 0);
		av_opt_set_int(swr_ctx, "in_sample_rate", frame->sample_rate, 0);
		av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", (AVSampleFormat)frame->format, 0);
		av_opt_set_int(swr_ctx, "out_channel_count", codecContext->channels, 0);
		av_opt_set_int(swr_ctx, "out_sample_rate", codecContext->sample_rate, 0);
		av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", codecContext->sample_fmt, 0);

		// initialize the resampling context
		int ret;
		if ((ret = swr_init(swr_ctx)) < 0)
		{
			throw FFmpegException("Failed to initialize the resampling context", ret);
		}

	}

	void AudioFormatConverter::ProcessFrame(AVFrame* frame)
	{
		// initialize the resampler
		if (!initialized)
		{
			InitDelayed(frame);
			initialized = true;
		}

		int ret;
		ret = swr_convert_frame(swr_ctx, tmp_frame, frame);
		if (ret < 0)
		{
			throw FFmpegException("Error while converting audio frame to destination format", ret);
		}

		while (tmp_frame->nb_samples > 0)
		{
			AddToFifo(tmp_frame);
			ret = swr_convert_frame(swr_ctx, tmp_frame, NULL);
			if (ret < 0)
			{
				throw FFmpegException("Error while converting audio frame to destination format", ret);
			}
		}

		/* If we have enough samples for the encoder, we encode them.
		* At the end of the file, we pass the remaining samples to
		* the encoder. */
		bool finished = (frame == NULL);
		int fifoSize = av_audio_fifo_size(fifo);
		while (fifoSize >= converted_frame->nb_samples ||
			(finished && fifoSize > 0))
		{
			// Take one frame worth of audio samples from the FIFO buffer,
			 // encode it and write it to the output file. 
			PullConvertedFrameFromFifo();
			fifoSize = av_audio_fifo_size(fifo);
		}
	}

	void AudioFormatConverter::AddToFifo(AVFrame* frame)
	{
		// Make the FIFO as large as it needs to be to hold both, the old and the new samples.
		int ret;
		if ((ret = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame->nb_samples)) < 0)
		{
			throw FFmpegException("Could not reallocate FIFO", ret);
		}

		/* Store the new samples in the FIFO buffer. */
		if (av_audio_fifo_write(fifo, (void **)frame->extended_data, frame->nb_samples) < frame->nb_samples)
		{
			throw FFmpegException("Could not write data to FIFO");
		}
	}

	void AudioFormatConverter::PullConvertedFrameFromFifo()
	{
		/* Use the maximum number of possible samples per frame.
		 * If there is less than the maximum possible frame size in the FIFO
		 * buffer use this number. Otherwise, use the maximum possible frame size. */
		const int frame_size = FFMIN(av_audio_fifo_size(fifo), converted_frame->nb_samples);
		int data_written;
		converted_frame->nb_samples = frame_size;

		/* Read as many samples from the FIFO buffer as required to fill the frame.
		 * The samples are stored in the frame temporarily. */
		int ret;
		if ((ret = av_audio_fifo_read(fifo, (void **)converted_frame->data, frame_size)) < frame_size)
		{
			throw FFmpegException("Could not read data from FIFO", ret);
		}

		// send the frame to the encoder
		WriteCompleteConvertedFrame();
	}

	void AudioFormatConverter::WriteCompleteConvertedFrame()
	{
		AVRational inv_sample_rate;
		inv_sample_rate.num = 1;
		inv_sample_rate.den = codecContext->sample_rate;

		converted_frame->pts = av_rescale_q(samples_count, inv_sample_rate, codecContext->time_base);
		samples_count += converted_frame->nb_samples;

		output->WriteConvertedFrame(converted_frame);

		samplesInCurrentFrame = 0;
		int ret = av_frame_make_writable(converted_frame);
		if (ret < 0)
		{
			throw FFmpegException("Failed to make audio frame writable", ret);
		}

	}

}

