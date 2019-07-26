#pragma once

#include "ffmpeg.h"
#include "InputStream.h"
#include "Frame Sinks/AudioFrameSink.h"

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
