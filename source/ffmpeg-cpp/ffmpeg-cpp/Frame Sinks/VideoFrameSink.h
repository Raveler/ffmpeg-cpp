#pragma once

#include "FrameSink.h"

namespace ffmpegcpp
{
	class VideoFrameSink : public FrameSink
	{
	public:

		virtual void WriteFrame(AVFrame* frame, AVRational* timeBase) = 0;
	};
}
