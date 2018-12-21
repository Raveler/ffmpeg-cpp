#include "Demuxing/VideoInputStream.h"

#include "ffmpeg.h"
#include "FrameSinks/VideoFrameSink.h"

namespace ffmpegcpp
{
	VideoInputStream::VideoInputStream(VideoFrameSink* frameSink, AVStream* stream)
		: InputStream(stream)
	{
		SetFrameSink(frameSink);
	}

	void VideoInputStream::ConfigureCodecContext()
	{

	}
}

