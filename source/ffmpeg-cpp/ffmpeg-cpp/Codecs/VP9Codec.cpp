#include "VP9Codec.h"

namespace ffmpegcpp
{

	VP9Codec::VP9Codec(int width, int height, int framesPerSecond, AVPixelFormat format)
		: VideoCodec("libvpx-vp9", width, height, framesPerSecond, format)
	{

	}

	void VP9Codec::SetDeadline(const char* deadline)
	{
		SetOption("deadline", deadline);
	}

	void VP9Codec::SetCpuUsed(int cpuUsed)
	{
		SetOption("cpu-used", cpuUsed);
	}

	void VP9Codec::SetLossless(bool lossless)
	{
		SetOption("lossless", (lossless ? 1 : 0));
	}

	void VP9Codec::SetCrf(int crf)
	{
		SetOption("crf", crf);
	}


}