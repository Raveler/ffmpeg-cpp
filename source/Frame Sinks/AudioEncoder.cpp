#include "AudioEncoder.h"
#include "Muxing/AudioOutputStream.h"

#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{

	AudioEncoder::AudioEncoder(AudioCodec* codec, Muxer* muxer)
	{
		this->closedCodec = codec;

		// create an output stream
		output = new AudioOutputStream(muxer, codec);
		muxer->AddOutputStream(output);

		// this one is used to make sure we only allow one frame sink stream to be generated
		oneInputFrameSink = new OneInputFrameSink(this, AVMEDIA_TYPE_AUDIO);
	}

	AudioEncoder::AudioEncoder(AudioCodec* codec, Muxer* muxer, int bitRate)
		: AudioEncoder(codec, muxer)
	{
		finalBitRate = bitRate;
	}

	void AudioEncoder::OpenLazily(AVFrame* frame, StreamData* metaData)
	{
		// configure the parameters for the codec based on the frame, our settings & defaults
		int bitRate = finalBitRate;
		if (bitRate == -1) bitRate = 0; // default by the codecContext
		int sampleRate = closedCodec->GetDefaultSampleRate();
		AVSampleFormat format = closedCodec->GetDefaultSampleFormat();

		codec = closedCodec->Open(bitRate, format, sampleRate);

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
		if (codec != nullptr)
		{
			delete codec;
			codec = nullptr;
		}
		if (oneInputFrameSink != nullptr)
		{
			delete oneInputFrameSink;
			oneInputFrameSink = nullptr;
		}
	}

	FrameSinkStream* AudioEncoder::CreateStream()
	{
		return oneInputFrameSink->CreateStream();
	}

	void AudioEncoder::WriteFrame(int streamIndex, AVFrame* frame, StreamData* metaData)
	{
		// if we haven't opened the codec yet, we do it now!
		if (codec == nullptr)
		{
			OpenLazily(frame, metaData);
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

	void AudioEncoder::Close(int streamIndex)
	{
		if (codec == nullptr) return; // can't close if we were never opened

		// First flush the converter and the FIFO queue in it
		formatConverter->ProcessFrame(NULL);

		// then flush our encoder
		WriteConvertedFrame(NULL);
	}

	void AudioEncoder::PollCodecForPackets()
	{
		int ret = 0;
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
			output->WritePacket(pkt, codec);

			av_packet_unref(pkt);
		}
	}

	bool AudioEncoder::IsPrimed()
	{
		return output->IsPrimed();
	}
}

