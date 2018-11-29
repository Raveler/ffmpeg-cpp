#pragma once
#include "Codec.h"

namespace ffmpegcpp
{

	class VideoCodec : public Codec
	{
	public:

		VideoCodec(const char* codecName, int width, int height, int framesPerSecond, AVPixelFormat format);
		VideoCodec(AVCodecID codecId, int width, int height, int framesPerSecond, AVPixelFormat format);

	private:

		void InitContext(int width, int height, int framesPerSecond, AVPixelFormat format);
	};


}