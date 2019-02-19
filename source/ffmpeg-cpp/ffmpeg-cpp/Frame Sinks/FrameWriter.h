#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	class FrameWriter
	{
	public:

		virtual void WriteFrame(int streamIndex, AVFrame* frame, AVRational* timeBase) = 0;

		virtual void Close(int streamIndex) = 0;

		virtual bool IsPrimed() = 0;
	};


}
