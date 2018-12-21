#pragma once
#include "VideoCodec.h"

namespace ffmpegcpp
{

	class H265NVEncCodec : public VideoCodec
	{

	public:

		H265NVEncCodec();

		void SetPreset(const char* preset);
	};


}