#pragma once

#include "FrameSink.h"

namespace ffmpegcpp
{
	class AudioFrameSink : public FrameSink
	{
	public:

		virtual AVMediaType GetMediaType()
		{
			return AVMEDIA_TYPE_AUDIO;
		}

		virtual ~AudioFrameSink() {}
	};
}
