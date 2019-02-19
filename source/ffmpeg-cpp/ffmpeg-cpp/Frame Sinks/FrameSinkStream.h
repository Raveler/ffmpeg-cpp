#pragma once

#include "ffmpeg.h"
#include "FrameWriter.h"

namespace ffmpegcpp
{
	class FrameSinkStream
	{
	public:

		FrameSinkStream(FrameWriter* frameSink, int streamIdx);

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

		void Close();

		bool IsPrimed();

	private:

		FrameWriter* frameSink;
		int streamIndex;
	};
}
