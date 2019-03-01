#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	struct VideoStreamInfo
	{
		int id;
		AVRational frameRate;
		AVRational timeBase;
		const AVCodec* codec;
		float bitRate;

		AVPixelFormat format;
		const char* formatName;

		int width, height;
	};
}
