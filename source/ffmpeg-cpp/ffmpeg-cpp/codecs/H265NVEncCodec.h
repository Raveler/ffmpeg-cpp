#pragma once
#include "VideoCodec.h"

namespace ffmpegcpp
{

	class H265NVEncCodec : public VideoCodec
	{

	public:

		H265NVEncCodec(int width, int height, int framesPerSecond, AVPixelFormat format);

		void SetPreset(const char* preset);
	};


}