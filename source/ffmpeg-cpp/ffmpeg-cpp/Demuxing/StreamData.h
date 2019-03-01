#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	struct StreamData
	{
		AVRational timeBase;
		AVRational frameRate;
	};
}