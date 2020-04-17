#pragma once

#include "ffmpeg-cpp/ffmpeg.h"
#include "ffmpeg-cpp/Demuxing/InputStream.h"
#include "ffmpeg-cpp/Frame Sinks/VideoFrameSink.h"
#include "ffmpeg-cpp/Info/VideoStreamInfo.h"

namespace ffmpegcpp
{
	class VideoInputStream : public InputStream
	{

	public:

		VideoInputStream(AVFormatContext* format, AVStream* stream);
		~VideoInputStream();

		void AddStreamInfo(ContainerInfo* info);

	protected:

		virtual void ConfigureCodecContext();

	};
}
