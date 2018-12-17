#pragma once

#include "ffmpeg.h"
#include "Codecs/Codec.h"
#include "Muxing/OutputStream.h"
#include "Muxing/Muxer.h"

namespace ffmpegcpp
{
	class VideoOutputStream : public OutputStream
	{
	public:

		VideoOutputStream(Muxer* muxer, Codec* codec);

		void OpenStream(AVStream* stream, int containerFlags) override;

		void WritePacket(AVPacket* pkt, OpenCodec* openCodec) override;

		bool IsPrimed() override;

	protected:

		void PreparePacketForMuxer(AVPacket* pkt) override;

	private:

		void LazilyInitialize(OpenCodec* openCodec);

		AVStream* stream;

		bool initialized = false;

		AVRational codecTimeBase;
	};
}
