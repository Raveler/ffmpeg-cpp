#pragma once
#include "ffmpeg-cpp/Codecs/VideoCodec.h"

namespace ffmpegcpp
{

	class H264NVEncCodec : public VideoCodec
	{

	public:

		H264NVEncCodec();

		void SetPreset(const char* preset);
	};


}
