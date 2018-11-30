#pragma once

#include "ffmpeg.h"
#include "OutputStream.h"
#include "FrameSink.h"

namespace ffmpegcpp
{
	class RawVideoSource
	{

	public:

		RawVideoSource(int width, int height, AVPixelFormat pixelFormat, int framesPerSecond, int bytesPerRow, FrameSink* output);
		RawVideoSource(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, int bytesPerRow, FrameSink* output);
		~RawVideoSource();

		void WriteFrame(void* data);

	private:

		void Init(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, int bytesPerRow, FrameSink* output);

		AVPixelFormat sourcePixelFormat;
		AVRational timeBase;

		FrameSink* output;

		AVFrame* frame;

		struct SwsContext* swsContext;
	};
}
