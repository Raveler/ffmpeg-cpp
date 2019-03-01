#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	enum StreamType
	{
		Audio,
		Video
	};

	struct StreamInfo
	{
		StreamType type;
		float frameRate;
		float timeBase;
		float bitRate;
	};


}
