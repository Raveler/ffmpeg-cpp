#pragma once

#include "ffmpeg.h"
#include "Codec.h"

namespace ffmpegcpp
{
	class OutputStream
	{
	public:

		OutputStream(OpenCodec* codec);

		void OpenStream(AVFormatContext* containerContext);

		void WritePacket(AVPacket* pkt);

		OpenCodec* GetCodec();

	private:

		OpenCodec* codec;

		AVFormatContext* containerContext;

		AVStream* stream;
	};
}
