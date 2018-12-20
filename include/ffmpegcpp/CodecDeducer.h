#pragma once

#include <string>

enum AVCodecID;
struct AVCodec;

namespace ffmpegcpp
{
	class CodecDeducer
	{
	public:

		static AVCodec* DeduceEncoderFromFilename(const std::string & fileName);

		static AVCodec* DeduceEncoder(AVCodecID codecId);
		static AVCodec* DeduceEncoder(const std::string & codecName);

		static AVCodec* DeduceDecoder(AVCodecID codecId);
		static AVCodec* DeduceDecoder(const std::string & codecName);
	};

}
