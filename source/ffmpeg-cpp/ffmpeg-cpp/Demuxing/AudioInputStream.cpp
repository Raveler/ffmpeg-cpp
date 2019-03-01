#include "AudioInputStream.h"

namespace ffmpegcpp
{
	AudioInputStream::AudioInputStream(AudioFrameSink* frameSink, AVFormatContext* format, AVStream* stream)
		: InputStream(format, stream)
	{
		SetFrameSink(frameSink);
	}

	void AudioInputStream::ConfigureCodecContext()
	{

		// try to guess the channel layout for the decoder
		if (!codecContext->channel_layout)
		{
			codecContext->channel_layout = av_get_default_channel_layout(codecContext->channels);
		}
	}

	AudioInputStream::~AudioInputStream()
	{
	}
}



