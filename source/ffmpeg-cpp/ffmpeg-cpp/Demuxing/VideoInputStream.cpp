#include "VideoInputStream.h"

namespace ffmpegcpp
{
	VideoInputStream::VideoInputStream(VideoFrameSink* frameSink, AVFormatContext* format, AVStream* stream)
		: InputStream(format, stream)
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

