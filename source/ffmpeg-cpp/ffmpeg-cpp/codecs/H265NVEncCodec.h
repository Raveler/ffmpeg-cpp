#pragma once
#include "ffmpeg-cpp/Codecs/VideoCodec.h"

namespace ffmpegcpp
{

	class H265NVEncCodec : public VideoCodec
	{

	public:

		H265NVEncCodec();

		void SetPreset(const char* preset);
	};


}
