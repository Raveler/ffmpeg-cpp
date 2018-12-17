#pragma once

#include "FrameSinks/FrameSink.h"

namespace ffmpegcpp
{
	class AudioFrameSink : public FrameSink
	{
	public:

		virtual ~AudioFrameSink() {}
	};
}
