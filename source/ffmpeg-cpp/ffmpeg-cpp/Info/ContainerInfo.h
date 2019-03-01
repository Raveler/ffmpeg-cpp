#pragma once

#include "ffmpeg.h"
#include "StreamInfo.h"

namespace ffmpegcpp
{
	struct ContainerInfo
	{
		float duration;
		float start;
		float bitRate;

		int nStreams;
		StreamInfo* streams;

	};


}
