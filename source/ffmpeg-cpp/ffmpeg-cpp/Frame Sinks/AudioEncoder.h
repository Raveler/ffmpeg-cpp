#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "Frame Sinks/AudioFrameSink.h"
#include "Codecs/AudioCodec.h"
#include "ConvertedAudioProcessor.h"
#include "AudioFormatConverter.h"
#include "Muxing/Muxer.h"
#include "Muxing/OutputStream.h"
#include "OneInputFrameSink.h"

namespace ffmpegcpp
{
	class AudioEncoder : public AudioFrameSink, public ConvertedAudioProcessor, public FrameWriter
	{
	public:
		AudioEncoder(AudioCodec* codec, Muxer* muxer);
		AudioEncoder(AudioCodec* codec, Muxer* muxer, int bitRate);
		virtual ~AudioEncoder();

		FrameSinkStream* CreateStream();
		void WriteFrame(int streamIndex, AVFrame* frame, StreamData* metaData);
		void Close(int streamIndex);

		virtual void WriteConvertedFrame(AVFrame* frame);

		bool IsPrimed();

	private:

		void OpenLazily(AVFrame* frame, StreamData* metaData);

		void CleanUp();

		void PollCodecForPackets();

		OutputStream* output;

		AudioCodec* closedCodec;

		AudioFormatConverter *formatConverter = nullptr;
		OpenCodec* codec = nullptr;
		AVPacket* pkt = nullptr;

		OneInputFrameSink* oneInputFrameSink = nullptr;

		int frameNumber = 0;

		int finalBitRate = -1;
	};
}

