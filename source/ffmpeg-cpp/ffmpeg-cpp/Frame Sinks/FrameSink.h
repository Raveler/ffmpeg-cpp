#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	class FrameSink
	{
	public:

		virtual void WriteFrame(AVFrame* frame, AVRational* timeBase) = 0;
	};


}
