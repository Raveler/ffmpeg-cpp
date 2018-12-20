#pragma once
#include "VideoCodec.h"

#include <string>

namespace ffmpegcpp
{

	class H265NVEncCodec : public VideoCodec
	{

	public:

		H265NVEncCodec();

		void SetPreset(const std::string & preset);
	};


}