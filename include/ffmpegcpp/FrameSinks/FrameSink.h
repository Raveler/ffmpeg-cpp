#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	class FrameSink
	{
	public:

		virtual void WriteFrame(AVFrame* frame, AVRational* timeBase) = 0;

		virtual void Close() = 0;

		virtual bool IsPrimed() const = 0;
		
		virtual ~FrameSink() {}
	};


}
