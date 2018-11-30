#pragma once

#include "ffmpeg.h"

#include "Frame Sinks/VideoFrameSink.h"
#include "Codecs/Codec.h"
#include "OutputStream.h"

namespace ffmpegcpp
{
	class VideoEncoder : public VideoFrameSink
	{
	public:
		VideoEncoder(OutputStream* output);
		~VideoEncoder();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

		AVPixelFormat GetRequiredPixelFormat();

	private:

		OpenCodec* codec;
		OutputStream* output;

		AVPacket* pkt;

		int frameNumber = 0;
	};
}

