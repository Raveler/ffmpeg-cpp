#include "Muxing/OutputStream.h"

#include "Codecs/Codec.h"
#include "ffmpeg.h"
#include "FFmpegException.h"
#include "Muxing/Muxer.h"

#include <algorithm>

using namespace std;

namespace ffmpegcpp
{
	OutputStream::OutputStream(Muxer* muxer, Codec* codec)
	{
		this->muxer = muxer;
		this->codec = codec;
	}

	void OutputStream::SendPacketToMuxer(AVPacket* pkt)
	{
		// if the muxer is primed, we submit the packet for real
		if (muxer->IsPrimed())
		{
			// drain the queue
			DrainPacketQueue();

			// send this packet
			PreparePacketForMuxer(pkt);
			muxer->WritePacket(pkt);
		}

		// otherwise, we queue the packet
		else
		{
			auto tmp_pkt = MakeFFmpegResource<AVPacket>(av_packet_alloc());
			if (!tmp_pkt)
			{
				throw FFmpegException("Failed to allocate packet");
			}
			av_packet_ref(tmp_pkt.get(), pkt);
			packetQueue.emplace_back(std::move(tmp_pkt));
		}
	}

	void OutputStream::DrainPacketQueue()
	{
		if (packetQueue.size() > 0) printf("Drain %d packets from the packet queue...", packetQueue.size());
		std::for_each(begin(packetQueue), end(packetQueue), [this](auto & tmp_pkt)
		{
			// Write the compressed frame to the media file
			PreparePacketForMuxer(tmp_pkt.get());
			muxer->WritePacket(tmp_pkt.get());

			// Release the packet
			av_packet_unref(tmp_pkt.get());
		});
		packetQueue.clear();
	}
}
