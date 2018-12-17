#pragma once

#include "FrameSinks/FrameSink.h"

namespace ffmpegcpp
{
	class VideoFrameSink : public FrameSink
	{
	public:

		virtual ~VideoFrameSink() {}
	};
}
