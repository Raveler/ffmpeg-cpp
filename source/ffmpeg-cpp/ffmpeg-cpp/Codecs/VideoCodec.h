#pragma once

#include "ffmpeg-cpp/Codecs/Codec.h"
#include "ffmpeg-cpp/OpenCodec.h"

namespace ffmpegcpp
{

	class VideoCodec : public Codec
	{
	public:

		VideoCodec(const char* codecName);
		VideoCodec(AVCodecID codecId);
		virtual ~VideoCodec();

		OpenCodec* Open(int width, int height, AVRational* frameRate, AVPixelFormat format);

		// This maps to the qscale parameter so should be in the range [0,31].
		void SetQualityScale(int qscale);

		bool IsPixelFormatSupported(AVPixelFormat format);
		bool IsFrameRateSupported(AVRational* frameRate);

		AVPixelFormat GetDefaultPixelFormat();
		AVRational GetClosestSupportedFrameRate(AVRational frameRate);

	};


}
