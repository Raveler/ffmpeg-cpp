#include "AudioInputStream.h"

namespace ffmpegcpp
{

	AudioInputStream::AudioInputStream(AudioFrameSink* frameSink, AVStream* stream)
		: InputStream(stream)
	{
		SetFrameSink(frameSink);
	}

	AudioInputStream::~AudioInputStream()
	{
	}
}



