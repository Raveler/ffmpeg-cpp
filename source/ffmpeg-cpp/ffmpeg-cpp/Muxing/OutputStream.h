#pragma once

#include "ffmpeg-cpp/ffmpeg.h"
#include "ffmpeg-cpp/Codecs/Codec.h"
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

		void DrainPacketQueue();

	protected:

		virtual void PreparePacketForMuxer(AVPacket* packet) = 0;

		void SendPacketToMuxer(AVPacket* packet);


		Codec* codec;

		std::vector<AVPacket*> packetQueue;

	private:

		Muxer* muxer;
	};
}
