#pragma once

#include "ffmpeg.h"

#include "FrameSink.h"
#include "Codec.h"
#include "OutputStream.h"

namespace ffmpegcpp
{
	class Encoder : public FrameSink
	{
	public:
		Encoder(OutputStream* output);
		~Encoder();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

	private:

		OpenCodec* codec;
		OutputStream* output;

		AVPacket* pkt;

		int frameNumber = 0;
	};
}

