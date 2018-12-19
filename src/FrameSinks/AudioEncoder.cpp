#include "FrameSinks/AudioEncoder.h"

#include "AudioFormatConverter.h"
#include "Codecs/AudioCodec.h"
#include "ffmpeg.h"
#include "FFmpegException.h"
#include "Muxing/AudioOutputStream.h"
#include "Muxing/Muxer.h"
#include "Muxing/OutputStream.h"
#include "OpenCodec.h"
#include "std.h"

using namespace std;

namespace ffmpegcpp
{

	AudioEncoder::AudioEncoder(AudioCodec* codec, Muxer* muxer)
	{
		this->closedCodec = codec;

		// create an output stream
		output = std::make_unique<AudioOutputStream>(muxer, codec);
		muxer->AddOutputStream(output.get());
	}

	AudioEncoder::AudioEncoder(AudioCodec* codec, Muxer* muxer, int bitRate)
		: AudioEncoder(codec, muxer)
	{
		finalBitRate = bitRate;
	}

	void AudioEncoder::OpenLazily(AVFrame* frame, AVRational* timeBase) 
	{
		// configure the parameters for the codec based on the frame, our settings & defaults
		int bitRate = finalBitRate;
		if (bitRate == -1) bitRate = 0; // default by the codecContext
		int sampleRate = closedCodec->GetDefaultSampleRate();
		AVSampleFormat format = closedCodec->GetDefaultSampleFormat();

		codec = closedCodec->Open(bitRate, format, sampleRate);

		pkt = MakeFFmpegResource<AVPacket>(av_packet_alloc());
		if (!pkt)
		{
			throw FFmpegException("Failed to allocate packet");
		}

		try
		{
			formatConverter = std::make_unique<AudioFormatConverter>(this, codec->GetContext());
		}
		catch (FFmpegException e)
		{
			throw e;
		}
	}

	void AudioEncoder::WriteFrame(AVFrame* frame, AVRational* timeBase)
	{
		// if we haven't opened the codec yet, we do it now!
		if (codec == nullptr)
		{
			OpenLazily(frame, timeBase);
		}

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
		PollCodecForPackets();
	}

	void AudioEncoder::Close()
	{
		if (codec == nullptr) return; // can't close if we were never opened

		// First flush the converter and the FIFO queue in it
		formatConverter->ProcessFrame(nullptr);

		// then flush our encoder
		WriteConvertedFrame(nullptr);
	}

	void AudioEncoder::PollCodecForPackets()
	{
		int ret = 0;
		while (ret >= 0)
		{
			ret = avcodec_receive_packet(codec->GetContext(), pkt.get());
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
			output->WritePacket(pkt.get(), codec.get());

			av_packet_unref(pkt.get());
		}
	}

	bool AudioEncoder::IsPrimed() const
	{
		return output->IsPrimed();
	}
}

