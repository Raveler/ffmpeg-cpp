#pragma once

#include "ffmpeg.h"
#include "OutputStream.h"
#include "Frame Sinks/VideoFrameSink.h"

namespace ffmpegcpp
{
	class RawVideoSource
	{

	public:

		RawVideoSource(int width, int height, AVPixelFormat pixelFormat, int framesPerSecond, VideoFrameSink* output);
		RawVideoSource(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, VideoFrameSink* output);
		~RawVideoSource();

		void WriteFrame(void* data, int bytesPerRow);

	private:

		void Init(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, VideoFrameSink* output);
		void CleanUp();

		AVPixelFormat sourcePixelFormat;
		AVRational timeBase;

		VideoFrameSink* output;

		AVFrame* frame = nullptr;

		struct SwsContext* swsContext = NULL;
	};
}
