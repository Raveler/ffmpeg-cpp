#pragma once

#include "FFmpegResource.h"
#include "FrameSinks/AudioFrameSink.h"
#include "ConvertedAudioProcessor.h"

#include <memory>

struct AVFrame;
struct AVPacket;
struct AVRational;

namespace ffmpegcpp
{
	class AudioCodec;
	class AudioFormatConverter;
	class Muxer;
	class OpenCodec;
	class OutputStream;

	class AudioEncoder : public AudioFrameSink, public ConvertedAudioProcessor
	{
	public:
		AudioEncoder(AudioCodec* codec, Muxer* muxer);
		AudioEncoder(AudioCodec* codec, Muxer* muxer, int bitRate);

		void WriteFrame(AVFrame* frame, AVRational* timeBase) override;
		void Close() override;

		void WriteConvertedFrame(AVFrame* frame) override;

		bool IsPrimed() override;

	private:

		void OpenLazily(AVFrame* frame, AVRational* timeBase);

		void PollCodecForPackets();

		std::unique_ptr<OutputStream> output;

		AudioCodec* closedCodec;

		std::unique_ptr<AudioFormatConverter> formatConverter;
		std::unique_ptr<OpenCodec> codec;
		FFmpegResource<AVPacket> pkt;

		int frameNumber = 0;

		int finalBitRate = -1;
	};
}

