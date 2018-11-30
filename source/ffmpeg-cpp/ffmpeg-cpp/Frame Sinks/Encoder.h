#pragma once

#include "ffmpeg.h"

#include "Frame Sinks/FrameSink.h"
#include "Codecs/Codec.h"
#include "OutputStream.h"

namespace ffmpegcpp
{
	class Encoder : public FrameSink
	{
	public:
		Encoder(OutputStream* output);
		~Encoder();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

		AVPixelFormat GetRequiredPixelFormat();

	private:

		OpenCodec* codec;
		OutputStream* output;

		AVPacket* pkt;

		int frameNumber = 0;
	};
}

