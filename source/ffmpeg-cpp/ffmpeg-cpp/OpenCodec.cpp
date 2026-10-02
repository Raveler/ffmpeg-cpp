#include "OpenCodec.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	OpenCodec::OpenCodec(AVCodecContext* context)
	{
		if (!avcodec_is_open(context))
		{
			throw FFmpegException("Codec context for " + string(context->codec->name) + " hasn't been opened yet");
		}

		this->context = context;
	}

	OpenCodec::~OpenCodec()
	{
		avcodec_free_context(&context);
	}

	AVCodecContext* OpenCodec::GetContext()
	{
		return context;
	}
}
