#pragma once

#include "FrameSinks/FrameSink.h"

namespace ffmpegcpp
{
	class VideoFrameSink : public FrameSink
	{
	public:

		virtual void WriteFrame(AVFrame* frame, AVRational* timeBase) = 0;

		virtual ~VideoFrameSink() {}
	};
}
