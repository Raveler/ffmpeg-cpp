#pragma once

#include "ffmpeg-cpp/ffmpeg.h"

namespace ffmpegcpp
{
	struct StreamData
	{
		AVMediaType type;

		AVRational timeBase;
		AVRational frameRate;
	};
}
