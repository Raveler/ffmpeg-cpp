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

		try
		{
			formatConverter = new AudioFormatConverter(this, codec->GetContext());
		}
		catch (FFmpegException e)
		{
			CleanUp();
			throw e;
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
		if (formatConverter != nullptr)
		{
			delete formatConverter;
			formatConverter = nullptr;
		}
	}

	void AudioEncoder::WriteFrame(AVFrame* frame, AVRational* timeBase)
	{
		// set the PTS properly
		frame->pts = frameNumber;
		frameNumber += frame->nb_samples;

		// guess the channel layout if necessary
		if (frame->channel_layout == 0)
		{
			frame->channel_layout = av_get_default_channel_layout(frame->channels);
		}

		// convert the frame to the destination format
		formatConverter->ProcessFrame(frame);
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

