#pragma once

#include "Muxing/OutputStream.h"

struct AVPacket;
struct AVRational;
struct AVStream;

namespace ffmpegcpp
{
	class Codec;
	class Muxer;
	class OpenCodec;

	class AudioOutputStream : public OutputStream
	{
	public:

		AudioOutputStream(Muxer* muxer, Codec* codec);

		void OpenStream(AVStream* stream, int containerFlags) override;

		void WritePacket(AVPacket* pkt, OpenCodec* openCodec) override;

		bool IsPrimed()  const override;

	protected:

		void PreparePacketForMuxer(AVPacket* pkt) override;

	private:

		void LazilyInitialize(OpenCodec* openCodec);

		AVStream* stream;

		bool initialized = false;

		AVRational codecTimeBase;
	};
}
