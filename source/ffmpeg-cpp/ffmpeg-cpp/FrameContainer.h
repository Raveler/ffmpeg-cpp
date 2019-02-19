#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	class FrameContainer
	{
	public:

		FrameContainer(AVFrame* frame, AVRational* timeBase);
		~FrameContainer();

		AVFrame* GetFrame();
		AVRational* GetTimeBase();

	private:

		AVFrame* frame;
		AVRational *timeBase;
	};

}

