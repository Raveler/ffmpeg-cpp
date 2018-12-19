#pragma once

#include "Muxing/OutputStream.h"

struct AVPacket;
struct AVStream;

namespace ffmpegcpp
{
	class Codec;
	class Muxer;
	class OpenCodec;

	class VideoOutputStream : public OutputStream
	{
	public:

		VideoOutputStream(Muxer* muxer, Codec* codec);

		void OpenStream(AVStream* stream, int containerFlags) override;

		void WritePacket(AVPacket* pkt, OpenCodec* openCodec) override;

		bool IsPrimed() const override;

	protected:

		void PreparePacketForMuxer(AVPacket* pkt) override;

	private:

		void LazilyInitialize(OpenCodec* openCodec);

		AVStream* stream;

		bool initialized = false;

		AVRational codecTimeBase;
	};
}
