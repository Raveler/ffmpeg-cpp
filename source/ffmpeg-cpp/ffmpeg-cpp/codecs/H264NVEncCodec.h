#pragma once
#include "VideoCodec.h"

namespace ffmpegcpp
{

	class H264NVEncCodec : public VideoCodec
	{

	public:

		H264NVEncCodec(int width, int height, int framesPerSecond, AVPixelFormat format);

		void SetPreset(const char* preset);
	};


}