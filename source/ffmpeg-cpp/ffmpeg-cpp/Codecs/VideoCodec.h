#pragma once

#include "Codec.h"
#include "OpenCodec.h"

namespace ffmpegcpp
{

	class VideoCodec : public Codec
	{
	public:

		VideoCodec(const char* codecName);
		VideoCodec(AVCodecID codecId);
		virtual ~VideoCodec();

		OpenCodec* Open(int width, int height, AVRational* frameRate, AVPixelFormat format);

		bool IsPixelFormatSupported(AVPixelFormat format);
		bool IsFrameRateSupported(AVRational* frameRate);

		AVPixelFormat GetDefaultPixelFormat();

	};


}