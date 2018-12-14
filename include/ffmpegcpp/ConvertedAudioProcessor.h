#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	class ConvertedAudioProcessor
	{
	public:

		virtual void WriteConvertedFrame(AVFrame* convertedFrame) = 0;
	};

}

