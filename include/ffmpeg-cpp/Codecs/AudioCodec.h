#pragma once

#include "Codec.h"
#include "OpenCodec.h"

namespace ffmpegcpp
{

	class AudioCodec : public Codec
	{
	public:

		AudioCodec(const char* codecName);
		AudioCodec(AVCodecID codecId);
		virtual ~AudioCodec();

		OpenCodec* Open(int bitRate, AVSampleFormat format, int sampleRate);

		bool IsFormatSupported(AVSampleFormat format);
		bool IsChannelsSupported(int channels);
		bool IsSampleRateSupported(int sampleRate);

		AVSampleFormat GetDefaultSampleFormat();
		int GetDefaultSampleRate();

	};


}