#include "H265NVEncCodec.h"

namespace ffmpegcpp
{

	H265NVEncCodec::H265NVEncCodec(int width, int height, int framesPerSecond, AVPixelFormat format)
		: VideoCodec("hevc_nvenc", width, height, framesPerSecond, format)
	{

	}
	void H265NVEncCodec::SetPreset(const char* preset)
	{
		SetOption("preset", preset);
	}
}