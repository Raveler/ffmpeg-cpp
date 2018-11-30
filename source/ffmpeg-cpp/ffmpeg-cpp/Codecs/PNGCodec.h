#pragma once
#include "VideoCodec.h"

namespace ffmpegcpp
{

	class PNGCodec : public VideoCodec
	{

	public:

		PNGCodec(int width, int height, int framesPerSecond);

		void SetCompressionLevel(int compressionLevel);
	};


}