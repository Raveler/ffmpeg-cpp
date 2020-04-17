#pragma once

#include "ffmpeg-cpp/ffmpeg.h"

namespace ffmpegcpp
{
	class CodecDeducer
	{
	public:

		static AVCodec* DeduceEncoderFromFilename(const char* fileName);

		static AVCodec* DeduceEncoder(AVCodecID codecId);
		static AVCodec* DeduceEncoder(const char* codecName);

		static AVCodec* DeduceDecoder(AVCodecID codecId);
		static AVCodec* DeduceDecoder(const char* codecName);
	};

}
