#pragma once

#include "ffmpeg.h"

#include "FrameWriter.h"
#include "FrameSink.h"


namespace ffmpegcpp
{
	class OneInputFrameSink : public FrameSink
	{
	public:

		OneInputFrameSink(FrameWriter* writer);
		~OneInputFrameSink();

		FrameSinkStream* CreateStream();


	private:

		int nStreamsGenerated = 0;

		FrameWriter* writer;

		FrameSinkStream* stream = nullptr;
	};
}

