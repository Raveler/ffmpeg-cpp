#pragma once

#include "ffmpeg.h"
#include "Demuxing/InputStream.h"
#include "FrameSinks/VideoFrameSink.h"

namespace ffmpegcpp
{
	class VideoInputStream : public InputStream
	{

	public:

		VideoInputStream(VideoFrameSink* frameSink, AVStream* stream);
		~VideoInputStream();

	protected:

		void ConfigureCodecContext() override;

	};
}
