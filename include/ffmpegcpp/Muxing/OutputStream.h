#pragma once

#include "FFmpegResource.h"

#include <vector>

struct AVPacket;
struct AVStream;

namespace ffmpegcpp
{
	class Codec;
	class Muxer;
	class OpenCodec;

	class OutputStream
	{
	public:

		OutputStream(Muxer* muxer, Codec* codec);

		virtual void OpenStream(AVStream* stream, int containerFlags) = 0;

		virtual void WritePacket(AVPacket* pkt, OpenCodec* openCodec) = 0;

		virtual bool IsPrimed() const = 0;

		void DrainPacketQueue();

	protected:

		virtual void PreparePacketForMuxer(AVPacket* packet) = 0;

		void SendPacketToMuxer(AVPacket* packet);

		Codec* codec;

		std::vector<FFmpegResource<AVPacket>> packetQueue;

	private:

		Muxer* muxer;
	};
}
