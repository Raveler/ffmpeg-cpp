#pragma once
#include "VideoCodec.h"

#include <string>

namespace ffmpegcpp
{

	class H264NVEncCodec : public VideoCodec
	{

	public:

		H264NVEncCodec();

		void SetPreset(const std::string & preset);
	};


}