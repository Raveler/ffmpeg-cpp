#include "AudioFormatConverter.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{
	AudioFormatConverter::AudioFormatConverter(ConvertedAudioProcessor* writer, AVCodecContext* codecContext)
	{
		this->output = writer;
		this->codecContext = codecContext;

		if (converted_frame != nullptr)
		{
			av_frame_free(&converted_frame);
		}

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
		if (swr_ctx != nullptr)
		{
			swr_free(&swr_ctx);
			swr_ctx = nullptr;
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

		ret = swr_convert_frame(swr_ctx, NULL, frame);
		if (ret < 0)
		{
			throw FFmpegException("Error while converting audio frame to destination format", ret);
		}

		// we don't need the old frame anymore
		av_frame_unref(frame);

		int64_t delay = swr_get_delay(swr_ctx, in_sample_rate);
		int64_t dst_nb_samples = av_rescale_rnd(delay,
			out_sample_rate, in_sample_rate, AV_ROUND_UP);

		while (dst_nb_samples > converted_frame->nb_samples)
		{
			// when we pass a frame to the encoder, it may keep a reference to it
			// internally;
			// make sure we do not overwrite it here
			ret = av_frame_make_writable(converted_frame);
			if (ret < 0)
			{
				throw FFmpegException("Failed to make audio frame writable", ret);
			}

			ret = swr_convert_frame(swr_ctx, converted_frame, NULL);
			if (ret < 0)
			{
				throw FFmpegException("Error while converting audio frame to destination format", ret);
			}

			WriteCompleteConvertedFrame();

			delay = swr_get_delay(swr_ctx, in_sample_rate);
			dst_nb_samples = av_rescale_rnd(delay,
				out_sample_rate, in_sample_rate, AV_ROUND_UP);
		}
		return;

		// Because the frames might not be aligned, we fill the current frame until it is full,
		// and then submit it. This means that multiple frames might fit into one converted_frame,
		// or that multiple converted_frames might come out of one frame.
		// Either way, only one call to swr_convert is necessary with frame, because internally
		// the samples that do not fit in the converted_frame buffer are buffered.
		// See: https://ffmpeg.org/doxygen/3.2/group__lswr.html
		int remainingSamples = converted_frame->nb_samples - samplesInCurrentFrame;

		// convert to destination format
		ret = swr_convert(swr_ctx,
			converted_frame->data + samplesInCurrentFrame, remainingSamples,
			(const uint8_t **)frame->data, frame->nb_samples);
		if (ret < 0)
		{
			throw FFmpegException("Error hile converting audio frame to destination format", ret);
		}
		int samplesWritten = ret;
		samplesInCurrentFrame += samplesWritten;

		// we don't need the old frame anymore
		av_frame_unref(frame);

		// if there are less samples written than there is room, this means the converted_frame is not full yet,
		// and we stop for now.
		if (samplesWritten < remainingSamples)
		{
			return;
		}

		// if we filled the converted_frame to the brim, we submit it and see if there are any other converted_frames
		// waiting out there.
		else
		{
			WriteCompleteConvertedFrame();

			// now keep pumping data into new converted_frames until we are done
			while (samplesWritten > 0)
			{
				int remainingSamples = converted_frame->nb_samples - samplesInCurrentFrame;
				ret = swr_convert(swr_ctx,
					converted_frame->data + samplesInCurrentFrame, remainingSamples,
					NULL, 0);
				if (ret < 0)
				{
					throw FFmpegException("Error hile converting audio frame to destination format", ret);
				}
				samplesWritten = ret;
				samplesInCurrentFrame += samplesWritten;

				if (samplesWritten >= remainingSamples)
				{
					WriteCompleteConvertedFrame();
				}
			}
		}
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

