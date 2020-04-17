#pragma once

#include "ffmpeg-cpp/ffmpeg.h"
#include "InputStream.h"
#include "ffmpeg-cpp/Frame Sinks/AudioFrameSink.h"

namespace ffmpegcpp
{
	class AudioInputStream : public InputStream
	{

	public:

		AudioInputStream(AVFormatContext* format, AVStream* stream);
		~AudioInputStream();

		void AddStreamInfo(ContainerInfo* info);

	protected:

		virtual void ConfigureCodecContext();
	};
}
