#pragma once

#include "ffmpeg.h"
#include "InputStream.h"
#include "Frame Sinks/AudioFrameSink.h"

namespace ffmpegcpp
{
	class AudioInputStream : public InputStream
	{

	public:

		AudioInputStream(AudioFrameSink* frameSink, AVStream* stream);
		~AudioInputStream();

	protected:

		virtual void ConfigureCodecContext();
	};
}
