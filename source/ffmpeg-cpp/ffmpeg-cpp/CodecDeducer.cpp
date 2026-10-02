#include "CodecDeducer.h"
#include "FFmpegException.h"
#include <string>

using namespace std;

namespace ffmpegcpp
{
	AVCodec* CodecDeducer::DeduceEncoder(const char* codecName)
	{
		AVCodec* codec = avcodec_find_encoder_by_name(codecName);
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

	AVCodec* CodecDeducer::DeduceDecoder(const char* codecName)
	{
		AVCodec* codec = avcodec_find_decoder_by_name(codecName);
		if (!codec)
		{
			throw FFmpegException("Codec " + string(codecName) + " not found");
		}
		return codec;
	}

	AVCodec* CodecDeducer::DeduceDecoder(AVCodecID codecId)
	{
		if (codecId == AV_CODEC_ID_NONE) return nullptr;
		AVCodec* codec = avcodec_find_decoder(codecId);
		if (!codec)
		{
			throw FFmpegException("Codec with id " + to_string((int)codecId) + " not found");
		}
		return codec;
	}

	AVCodec* CodecDeducer::DeduceEncoderFromFilename(const char* fileName)
	{
		throw FFmpegException("Not implemented yet");
	}
}
