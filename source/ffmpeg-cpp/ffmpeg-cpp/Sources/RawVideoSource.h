#pragma once

#include "ffmpeg.h"
#include "OutputStream.h"
#include "Frame Sinks/FrameSink.h"

namespace ffmpegcpp
{
	class RawVideoSource
	{

	public:

		RawVideoSource(int width, int height, AVPixelFormat pixelFormat, int framesPerSecond, FrameSink* output);
		RawVideoSource(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, FrameSink* output);
		~RawVideoSource();

		void WriteFrame(void* data, int bytesPerRow);

	private:

		void Init(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, FrameSink* output);

		AVPixelFormat sourcePixelFormat;
		AVRational timeBase;

		FrameSink* output;

		AVFrame* frame;

		struct SwsContext* swsContext = NULL;
	};
}
