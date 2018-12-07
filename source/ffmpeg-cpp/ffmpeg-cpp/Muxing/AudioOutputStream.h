#pragma once

#include "ffmpeg.h"
#include "Codecs/Codec.h"
#include "OutputStream.h"
#include "Muxer.h"

namespace ffmpegcpp
{
	class AudioOutputStream : public OutputStream
	{
	public:

		AudioOutputStream(Muxer* muxer, Codec* codec);

		virtual void OpenStream(AVStream* stream, int containerFlags);

		virtual void WritePacket(AVPacket* pkt, OpenCodec* openCodec);

		virtual bool IsPrimed();

	private:

		void LazilyInitialize(OpenCodec* openCodec);

		AVStream* stream;

		bool initialized = false;
	};
}
