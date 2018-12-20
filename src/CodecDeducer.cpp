#include "CodecDeducer.h"

#include "ffmpeg.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	AVCodec* CodecDeducer::DeduceEncoder(const std::string & codecName)
	{
		AVCodec* codec = avcodec_find_encoder_by_name(codecName.c_str());
		if (!codec)
		{
			throw FFmpegException("Codec " + string(codecName) + " not found");
		}
		return codec;
	}

	AVCodec* CodecDeducer::DeduceEncoder(AVCodecID codecId)
	{
		AVCodec* codec = avcodec_find_encoder(codecId);
		if (!codec)
		{
			throw FFmpegException("Codec with id " + to_string((int)codecId) + " not found");
		}
		return codec;
	}

	AVCodec* CodecDeducer::DeduceDecoder(const std::string & codecName)
	{
		AVCodec* codec = avcodec_find_decoder_by_name(codecName.c_str());
		if (!codec)
		{
			throw FFmpegException("Codec " + string(codecName) + " not found");
		}
		return codec;
	}

	AVCodec* CodecDeducer::DeduceDecoder(AVCodecID codecId)
	{
		AVCodec* codec = avcodec_find_decoder(codecId);
		if (!codec)
		{
			throw FFmpegException("Codec with id " + to_string((int)codecId) + " not found");
		}
		return codec;
	}

	AVCodec* CodecDeducer::DeduceEncoderFromFilename(const std::string & fileName)
	{
		throw FFmpegException("Not implemented yet");
	}
}
