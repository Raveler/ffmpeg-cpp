#include "JPGCodec.h"

namespace ffmpegcpp
{

	JPGCodec::JPGCodec()
		: VideoCodec(AV_CODEC_ID_MJPEG)
	{

		// we take the default image format of the codec
		codecContext->pix_fmt = codecContext->codec->pix_fmts[0];
	}

	void JPGCodec::SetCompressionLevel(int compressionLevel)
	{
		SetOption("compression_level", compressionLevel);
	}
}