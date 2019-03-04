#pragma once

#include <libavutil/rational.h>

namespace ffmpegcpp
{
	struct StreamData
	{
		AVRational timeBase;
		AVRational frameRate;
	};
}