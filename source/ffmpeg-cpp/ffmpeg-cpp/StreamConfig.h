#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	struct StreamConfig
	{
		AVCodecContext* codecContext;
	};
}