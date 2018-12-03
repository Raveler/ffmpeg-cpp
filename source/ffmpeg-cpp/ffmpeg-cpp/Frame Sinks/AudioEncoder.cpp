#include "AudioEncoder.h"

#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{

	AudioEncoder::AudioEncoder(AudioOutputStream* output)
	{
		this->codec = output->GetCodec();
		this->output = output;

		pkt = av_packet_alloc();
		if (!pkt)
		{
			CleanUp();
			throw FFmpegException("Failed to allocate packet");
		}

		// default sample count
		AVCodecContext* codecContext = codec->GetContext();
		int nb_samples;
		if (codecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
			nb_samples = 10000;
		else
			nb_samples = codecContext->frame_size;

		// the frame converted in the target sample format
		CreateConvertedFrame(nb_samples);
	}

	void AudioEncoder::CreateConvertedFrame(int nb_samples)
	{
		if (converted_frame != nullptr)
		{
			av_frame_free(&converted_frame);
		}

		AVCodecContext* codecContext = codec->GetContext();
		converted_frame = av_frame_alloc();
		int ret;
		if (!converted_frame)
		{
			CleanUp();
			throw FFmpegException("Error allocating an audio frame");
		}


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


	AudioEncoder::~AudioEncoder()
	{
		CleanUp();
	}

	void AudioEncoder::CleanUp()
	{
		if (pkt != nullptr)
		{
			av_packet_free(&pkt);
		}
		if (converted_frame != nullptr)
		{
			av_frame_free(&converted_frame);
		}
		if (swr_ctx != nullptr)
		{
			swr_free(&swr_ctx);
		}
	}

	void AudioEncoder::InitDelayed(AVFrame* frame, AVRational* timeBase)
	{
		// the codec context we are working with
		AVCodecContext* codecContext = codec->GetContext();

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

		/* initialize the resampling context */
		int ret;
		if ((ret = swr_init(swr_ctx)) < 0)
		{
			throw FFmpegException("Failed to initialize the resampling context", ret);
		}

	}

	void AudioEncoder::ConvertToDestinationFormat(AVFrame* frame, AVRational* timeBase)
	{
		AVCodecContext* codecContext = codec->GetContext();

		// convert samples from native format to destination codec format, using the resampler

		// when we pass a frame to the encoder, it may keep a reference to it
		// internally;
		// make sure we do not overwrite it here
		int writable = av_frame_is_writable(converted_frame);
		int ret = av_frame_make_writable(converted_frame);
		if (ret < 0)
		{
			throw FFmpegException("Failed to make audio frame writable", ret);
		}


		// convert to destination format
		ret = swr_convert(swr_ctx,
			converted_frame->data, converted_frame->nb_samples,
			(const uint8_t **)frame->data, frame->nb_samples);
		if (ret < 0)
		{
			throw FFmpegException("Error hile converting audio frame to destination format", ret);
		}

		AVRational inv_sample_rate;
		inv_sample_rate.num = 1;
		inv_sample_rate.den = codecContext->sample_rate;

		converted_frame->pts = av_rescale_q(samples_count, inv_sample_rate, codecContext->time_base);
		samples_count += converted_frame->nb_samples;

		// we don't need the old frame anymore
		av_frame_unref(frame);

		// write the converted frame to the encoder
		WriteConvertedFrame(converted_frame);

		// re-calculate the delay and "catch up" with the resampler by requesting more frames as long as we can fill entire converted frames,
		// since the codec will only accept full frames when encoding.
		int64_t delay = swr_get_delay(swr_ctx, in_sample_rate);
		int64_t dst_nb_samples = av_rescale_rnd(delay + frame->nb_samples,
			out_sample_rate, in_sample_rate, AV_ROUND_UP);
		while (dst_nb_samples >= converted_frame->nb_samples)
		{
			int ret = av_frame_make_writable(converted_frame);
			if (ret < 0)
			{
				throw FFmpegException("Failed to make audio frame writable", ret);
			}


			ret = swr_convert(swr_ctx,
				converted_frame->data, converted_frame->nb_samples,
				NULL, 0);

			if (ret < 0)
			{
				throw FFmpegException("Error while converting audio frame to destination format", ret);
			}

			AVRational inv_sample_rate;
			inv_sample_rate.num = 1;
			inv_sample_rate.den = codecContext->sample_rate;

			converted_frame->pts = av_rescale_q(samples_count, inv_sample_rate, codecContext->time_base);
			samples_count += converted_frame->nb_samples;

			WriteConvertedFrame(converted_frame);

			delay = swr_get_delay(swr_ctx, in_sample_rate);
			dst_nb_samples = av_rescale_rnd(delay + frame->nb_samples,
				out_sample_rate, in_sample_rate, AV_ROUND_UP);
		}
	}

	void AudioEncoder::WriteFrame(AVFrame* frame, AVRational* timeBase)
	{
		// initialize the resampler
		if (!initialized)
		{
			InitDelayed(frame, timeBase);
			initialized = true;
		}

		// set the PTS properly
		frame->pts = frameNumber;
		frameNumber += frame->nb_samples;

		// convert the frame to the destination format
		ConvertToDestinationFormat(frame, timeBase);
	}

	void AudioEncoder::WriteConvertedFrame(AVFrame* frame)
	{
		int ret = avcodec_send_frame(codec->GetContext(), frame);
		if (ret < 0)
		{
			throw FFmpegException("Error sending a frame for encoding", ret);
		}

		while (ret >= 0)
		{
			ret = avcodec_receive_packet(codec->GetContext(), pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			{
				return;
			}
			else if (ret < 0)
			{
				throw FFmpegException("Error during encoding", ret);
			}

			//printf("Write packet %3 (size=%5d)\n", data->pkt->pts, data->pkt->size);
			//fwrite(data->pkt->data, 1, data->pkt->size, data->f);
			output->WritePacket(pkt);

			av_packet_unref(pkt);
		}
	}

	AVSampleFormat AudioEncoder::GetRequiredSampleFormat()
	{
		return codec->GetContext()->sample_fmt;
	}

}

