#pragma once

#include "ffmpeg-cpp/ffmpeg.h"
#include "FrameSinkStream.h"

namespace ffmpegcpp
{
	class FrameSink
	{
	public:

		virtual FrameSinkStream* CreateStream() = 0;

		virtual AVMediaType GetMediaType() = 0;

		virtual ~FrameSink() {}
	};


}
