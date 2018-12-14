#pragma once
#include "VideoCodec.h"

namespace ffmpegcpp
{

	class PNGCodec : public VideoCodec
	{

	public:

		PNGCodec();

		void SetCompressionLevel(int compressionLevel);
	};


}