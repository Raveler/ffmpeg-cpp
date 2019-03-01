#include "AudioInputStream.h"

namespace ffmpegcpp
{
	AudioInputStream::AudioInputStream(AVFormatContext* format, AVStream* stream)
		: InputStream(format, stream)
	{
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

	void AudioInputStream::AddStreamInfo(ContainerInfo* containerInfo)
	{
		// TODO
	}
}



