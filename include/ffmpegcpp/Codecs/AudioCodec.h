#pragma once

#include "Codecs/Codec.h"

enum AVCodecID;
enum AVSampleFormat;

namespace ffmpegcpp
{
	class OpenCodec;

	class AudioCodec : public Codec
	{
	public:

		AudioCodec(const char* codecName);
		AudioCodec(AVCodecID codecId);

        std::unique_ptr<OpenCodec> Open(int bitRate, AVSampleFormat format, int sampleRate);

		bool IsFormatSupported(AVSampleFormat format) const;
		bool IsChannelsSupported(int channels) const;
		bool IsSampleRateSupported(int sampleRate) const;

		AVSampleFormat GetDefaultSampleFormat() const;
		int GetDefaultSampleRate() const;

	};


}