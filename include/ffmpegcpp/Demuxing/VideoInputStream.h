#pragma once

#include "Demuxing/InputStream.h"

struct AVStream;

namespace ffmpegcpp
{
	class VideoFrameSink;

	class VideoInputStream : public InputStream
	{

	public:

		VideoInputStream(VideoFrameSink* frameSink, AVStream* stream);

	protected:

		void ConfigureCodecContext() override;

	};
}
