#pragma once

#include "ffmpeg-cpp/ffmpeg.h"
#include "ffmpeg-cpp/Frame Sinks/VideoFrameSink.h"

namespace ffmpegcpp
{
	// RawVideoDataSource is used to feed raw memory to the system and process it.
	// You can use this if the video data comes from another source than the file system (ie rendering).
	class RawVideoDataSource
	{

	public:

		RawVideoDataSource(int width, int height, AVPixelFormat pixelFormat, int framesPerSecond, FrameSink* output);
		RawVideoDataSource(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, FrameSink* output);
		virtual ~RawVideoDataSource();

		void WriteFrame(void* data, int bytesPerRow);
		void Close();

		int GetWidth();
		int GetHeight();

		bool IsPrimed();

	private:

		void Init(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, FrameSink* output);
		void CleanUp();

		AVPixelFormat sourcePixelFormat;

		FrameSinkStream* output;

		StreamData metaData;

		AVFrame* frame = nullptr;
		struct SwsContext* swsContext = nullptr;
	};
}
