#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "Frame Sinks/AudioFrameSink.h"
#include "Codecs/AudioCodec.h"
#include "ConvertedAudioProcessor.h"
#include "AudioFormatConverter.h"
#include "Muxing/Muxer.h"
#include "Muxing/OutputStream.h"

namespace ffmpegcpp
{
	class AudioEncoder : public AudioFrameSink, public ConvertedAudioProcessor
	{
	public:
		AudioEncoder(AudioCodec* codec, Muxer* muxer);
		AudioEncoder(AudioCodec* codec, Muxer* muxer, int bitRate);
		virtual ~AudioEncoder();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);
		void Close();

		virtual void WriteConvertedFrame(AVFrame* frame);

		bool IsPrimed();

	private:

		void OpenLazily(AVFrame* frame, AVRational* timeBase);

		void CleanUp();

		void PollCodecForPackets();

		OutputStream* output;

		AudioCodec* closedCodec;

		AudioFormatConverter *formatConverter = nullptr;
		OpenCodec* codec = nullptr;
		AVPacket* pkt = nullptr;

		int frameNumber = 0;

		int finalBitRate = -1;
	};
}

