#pragma once

#include "ffmpeg-cpp/ffmpeg.h"

namespace ffmpegcpp
{
	class OpenCodec
	{
	public:

		OpenCodec(AVCodecContext* openCodecContext);
		~OpenCodec();

		AVCodecContext* GetContext();

	private:

		AVCodecContext* context;
	};


}
