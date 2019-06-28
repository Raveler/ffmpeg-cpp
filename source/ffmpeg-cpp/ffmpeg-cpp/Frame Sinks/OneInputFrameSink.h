#pragma once

#include "ffmpeg.h"

#include "FrameWriter.h"
#include "FrameSink.h"


namespace ffmpegcpp
{
	class OneInputFrameSink : public FrameSink
	{
	public:

		OneInputFrameSink(FrameWriter* writer, AVMediaType mediaType);
		~OneInputFrameSink();

		virtual AVMediaType GetMediaType();

		FrameSinkStream* CreateStream();


	private:

		int nStreamsGenerated = 0;

		FrameWriter* writer;

		FrameSinkStream* stream = nullptr;

		AVMediaType mediaType;
	};
}

