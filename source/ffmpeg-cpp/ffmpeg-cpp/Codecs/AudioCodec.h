#pragma once
#include "Codec.h"

namespace ffmpegcpp
{

	class AudioCodec : public Codec
	{
	public:

		AudioCodec(const char* codecName, int bitRate, AVSampleFormat format);
		AudioCodec(AVCodecID codecId, int bitRate, AVSampleFormat format);
		AudioCodec(const char* codecName);
		AudioCodec(AVCodecID codecId);

	private:

		void InitContextWithDefaults();

		void InitContext(int bitRate, AVSampleFormat format);
	};


}