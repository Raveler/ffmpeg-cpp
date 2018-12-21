#pragma once

#include "Codec.h"

#include <string>

enum AVCodecID;
enum AVSampleFormat;
struct AVRational;

namespace ffmpegcpp
{
	class OpenCodec;

	class VideoCodec : public Codec
	{
	public:

		VideoCodec(const std::string & codecName);
		VideoCodec(AVCodecID codecId);
		virtual ~VideoCodec();

        std::unique_ptr<OpenCodec> Open(int width, int height, AVRational* frameRate, AVPixelFormat format);

		// This maps to the qscale parameter so should be in the range [0,31].
		void SetQualityScale(int qscale);

		bool IsPixelFormatSupported(AVPixelFormat format) const;
		bool IsFrameRateSupported(AVRational* frameRate) const;

		AVPixelFormat GetDefaultPixelFormat() const;
		AVRational GetClosestSupportedFrameRate(AVRational frameRate) const;

	};


}