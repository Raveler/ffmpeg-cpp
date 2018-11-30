#include "PNGCodec.h"

namespace ffmpegcpp
{

	PNGCodec::PNGCodec(int width, int height, int framesPerSecond)
		: VideoCodec(AV_CODEC_ID_PNG, width, height, framesPerSecond, AV_PIX_FMT_NONE)
	{

		// we take the default image format of the codec
		codecContext->pix_fmt = codecContext->codec->pix_fmts[0];
	}

	void PNGCodec::SetCompressionLevel(int compressionLevel)
	{
		SetOption("compression_level", compressionLevel);
	}
}