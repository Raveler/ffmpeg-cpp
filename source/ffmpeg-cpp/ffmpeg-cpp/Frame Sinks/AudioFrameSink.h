#pragma once

#include "FrameSink.h"

namespace ffmpegcpp
{
	class AudioFrameSink : public FrameSink
	{
	public:

		virtual void WriteFrame(AVFrame* frame, AVRational* timeBase) = 0;
	};
}
