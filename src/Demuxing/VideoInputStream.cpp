#include "Demuxing/VideoInputStream.h"

namespace ffmpegcpp
{
	VideoInputStream::VideoInputStream(VideoFrameSink* frameSink, AVStream* stream)
		: InputStream(stream)
	{
		SetFrameSink(frameSink);
	}

	VideoInputStream::~VideoInputStream()
	{
	}

	void VideoInputStream::ConfigureCodecContext()
	{

	}
}

