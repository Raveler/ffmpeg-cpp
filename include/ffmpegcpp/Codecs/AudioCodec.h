#pragma once

#include "Codecs/Codec.h"

#include <string>

enum AVCodecID;
enum AVSampleFormat;

namespace ffmpegcpp
{
	class OpenCodec;

	class AudioCodec : public Codec
	{
	public:

		AudioCodec(const std::string & codecName);
		AudioCodec(AVCodecID codecId);

        std::unique_ptr<OpenCodec> Open(int bitRate, AVSampleFormat format, int sampleRate);

		bool IsFormatSupported(AVSampleFormat format) const;
		bool IsChannelsSupported(int channels) const;
		bool IsSampleRateSupported(int sampleRate) const;

		AVSampleFormat GetDefaultSampleFormat() const;
		int GetDefaultSampleRate() const;

	};


}