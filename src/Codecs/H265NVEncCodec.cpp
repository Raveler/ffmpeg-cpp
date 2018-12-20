#include "Codecs/H265NVEncCodec.h"

namespace ffmpegcpp
{

	H265NVEncCodec::H265NVEncCodec()
		: VideoCodec("hevc_nvenc")
	{

	}

	void H265NVEncCodec::SetPreset(const std::string & preset)
	{
		SetOption("preset", preset);
	}
}