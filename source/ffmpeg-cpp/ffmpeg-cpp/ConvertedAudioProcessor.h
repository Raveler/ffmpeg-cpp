#pragma once

#include "ffmpeg-cpp/ffmpeg.h"

namespace ffmpegcpp
{
	class ConvertedAudioProcessor
	{
	public:

		virtual void WriteConvertedFrame(AVFrame* convertedFrame) = 0;
	};

}

