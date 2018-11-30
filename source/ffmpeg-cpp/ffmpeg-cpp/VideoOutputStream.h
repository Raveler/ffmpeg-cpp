#pragma once

#include "ffmpeg.h"
#include "Codecs/Codec.h"
#include "OutputStream.h"

namespace ffmpegcpp
{
	class VideoOutputStream : public OutputStream
	{
	public:

		VideoOutputStream(OpenCodec* codec);

		virtual void OpenStream(AVFormatContext* containerContext);

		virtual void WritePacket(AVPacket* pkt);

	private:

		AVFormatContext* containerContext;

		AVStream* stream;
	};
}
