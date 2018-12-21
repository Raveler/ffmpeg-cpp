#pragma once

#include "FFmpegResource.h"

enum AVPixelFormat;
struct AVFrame;
struct AVRational;

namespace ffmpegcpp
{
	class VideoFrameSink;

	// RawVideoDataSource is used to feed raw memory to the system and process it.
	// You can use this if the video data comes from another source than the file system (ie rendering).
	class RawVideoDataSource
	{

	public:

		RawVideoDataSource(int width, int height, AVPixelFormat pixelFormat, int framesPerSecond, VideoFrameSink* output);
		RawVideoDataSource(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, VideoFrameSink* output);

		void WriteFrame(void* data, int bytesPerRow);
		void Close();

		int GetWidth() const;
		int GetHeight() const;

		bool IsPrimed() const;

	private:

		void Init(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, VideoFrameSink* output);

		AVPixelFormat sourcePixelFormat;
		AVRational timeBase;

		VideoFrameSink* output;

		FFmpegResource<AVFrame> frame;
		FFmpegResource<struct SwsContext> swsContext;
	};
}
