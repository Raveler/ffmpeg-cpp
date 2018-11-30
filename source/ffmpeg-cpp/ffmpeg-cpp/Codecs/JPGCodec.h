#pragma once
#include "VideoCodec.h"

namespace ffmpegcpp
{

	class JPGCodec : public VideoCodec
	{

	public:

		JPGCodec(int width, int height, int framesPerSecond);

		void SetCompressionLevel(int compressionLevel);
	};


}