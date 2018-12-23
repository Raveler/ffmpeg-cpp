
#include "CodecDeducer.h"
#include "Codecs/Codec.h"
#include "ffmpeg.h"
#include "FFmpegException.h"
#include "OpenCodec.h"

using namespace std;

namespace
{
	ffmpegcpp::FFmpegResource<AVCodecContext> LoadContext(AVCodec* codec)
	{
		auto codecContext = ffmpegcpp::MakeFFmpegResource<AVCodecContext>(avcodec_alloc_context3(codec));
		if (!codecContext)
		{
			throw ffmpegcpp::FFmpegException("Could not allocate video codec context for codec " + string(codec->name));
		}

		// copy the type
		codecContext->codec_type = codec->type;

		return codecContext;
	}

}

namespace ffmpegcpp
{
	Codec::Codec(const std::string & codecName)
	{
		
		AVCodec* codec = CodecDeducer::DeduceEncoder(codecName);
		codecContext = LoadContext(codec);
	}


	Codec::Codec(AVCodecID codecId)
	{
		AVCodec* codec = CodecDeducer::DeduceEncoder(codecId);
		codecContext = LoadContext(codec);
	}

	void Codec::SetOption(const std::string & name, const std::string & value)
	{
		av_opt_set(codecContext->priv_data, name.c_str(), value.c_str(), 0);
	}

	void Codec::SetOption(const std::string & name, int value)
	{
		av_opt_set_int(codecContext->priv_data, name.c_str(), value, 0);
	}

	void Codec::SetOption(const std::string & name, double value)
	{
		av_opt_set_double(codecContext->priv_data, name.c_str(), value, 0);
	}

    std::unique_ptr<OpenCodec> Codec::Open()
	{
		if (opened)
		{
			throw FFmpegException("You can only open a codec once");
		}

		int ret = avcodec_open2(codecContext.get(), codecContext->codec, nullptr);
		if (ret < 0)
		{
			throw FFmpegException("Could not open codecContext for codec", ret);
		}

		opened = true;

		return std::make_unique<OpenCodec>(codecContext.get());
	}

	void Codec::SetGlobalContainerHeader()
	{
		if (opened) throw FFmpegException("This flag should be set before opening the codec");
		codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
}
