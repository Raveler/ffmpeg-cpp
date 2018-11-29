#pragma once

#include "ffmpeg.h"

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
