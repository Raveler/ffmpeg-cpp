#include "VideoCodec.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{
	VideoCodec::VideoCodec(const char* codecName, int width, int height, int framesPerSecond, AVPixelFormat format)
		: Codec(codecName)
	{
		InitContext(width, height, framesPerSecond, format);
	}

	VideoCodec::VideoCodec(AVCodecID codecId, int width, int height, int framesPerSecond, AVPixelFormat format)
		: Codec(codecId)
	{
		InitContext(width, height, framesPerSecond, format);
	}

	void VideoCodec::InitContext(int width, int height, int framesPerSecond, AVPixelFormat format)
	{

		// resolution
		codecContext->width = width;
		codecContext->height = height;

		// FPS
		AVRational time_base;
		time_base.num = 1;
		time_base.den = framesPerSecond;
		codecContext->time_base = time_base;
		AVRational framerate;
		framerate.num = framesPerSecond;
		framerate.den = 1;
		codecContext->framerate = framerate;

		// format
		codecContext->pix_fmt = format;
	}
}
