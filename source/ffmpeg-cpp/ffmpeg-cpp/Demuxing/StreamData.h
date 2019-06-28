#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	struct StreamData
	{
		AVMediaType type;

		AVRational timeBase;
		AVRational frameRate;
	};
}