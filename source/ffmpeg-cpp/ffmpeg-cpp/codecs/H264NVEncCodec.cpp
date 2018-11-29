#include "H264NVEncCodec.h"

namespace ffmpegcpp
{

	H264NVEncCodec::H264NVEncCodec(int width, int height, int framesPerSecond, AVPixelFormat format)
		: VideoCodec("h264_nvenc", width, height, framesPerSecond, format)
	{

	}
	void H264NVEncCodec::SetPreset(const char* preset)
	{
		SetOption("preset", preset);
	}
}