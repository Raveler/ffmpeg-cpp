#pragma once

#include "Demuxing/InputStream.h"

struct AVStream;

namespace ffmpegcpp
{
	class AudioFrameSink;

	class AudioInputStream : public InputStream
	{

	public:

		AudioInputStream(AudioFrameSink* frameSink, AVStream* stream);
		~AudioInputStream();

	protected:

		void ConfigureCodecContext() override;
	};
}
