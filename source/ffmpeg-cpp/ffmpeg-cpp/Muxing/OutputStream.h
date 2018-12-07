#pragma once

#include "ffmpeg.h"
#include "Codecs/Codec.h"
#include "Muxer.h"

namespace ffmpegcpp
{
	class OutputStream
	{
	public:

		OutputStream(Muxer* muxer, Codec* codec);

		virtual void OpenStream(AVStream* stream, int containerFlags) = 0;

		virtual void WritePacket(AVPacket* pkt, OpenCodec* openCodec) = 0;

		virtual bool IsPrimed() = 0;

	protected:

		Muxer* muxer;

		Codec* codec;
	};
}
