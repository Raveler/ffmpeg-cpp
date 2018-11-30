#pragma once

#include "ffmpeg.h"
#include "Codecs/Codec.h"

namespace ffmpegcpp
{
	class OutputStream
	{
	public:

		OutputStream(OpenCodec* codec);

		virtual void OpenStream(AVFormatContext* containerContext) = 0;

		virtual void WritePacket(AVPacket* pkt) = 0;

		OpenCodec* GetCodec();

	protected:

		OpenCodec* codec;
	};
}
