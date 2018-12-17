#pragma once

#include "ffmpeg.h"
#include "Demuxing/InputStream.h"
#include "FrameSinks/AudioFrameSink.h"

namespace ffmpegcpp
{
	class AudioInputStream : public InputStream
	{

	public:

		AudioInputStream(AudioFrameSink* frameSink, AVStream* stream);
		~AudioInputStream();

	protected:

		void ConfigureCodecContext() override;
	};
}
