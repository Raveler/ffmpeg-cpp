#include "Muxing/OutputStream.h"
#include "FFmpegException.h"

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
			AVPacket* tmp_pkt = av_packet_alloc();
			if (!tmp_pkt)
			{
				throw FFmpegException("Failed to allocate packet");
			}
			av_packet_ref(tmp_pkt, pkt);
			packetQueue.push_back(tmp_pkt);
		}
	}

	void OutputStream::DrainPacketQueue()
	{
		if (packetQueue.size() > 0) printf("Drain %d packets from the packet queue...", packetQueue.size());
		for (int i = 0; i < packetQueue.size(); ++i)
		{
			AVPacket* tmp_pkt = packetQueue[i];

			// Write the compressed frame to the media file
			PreparePacketForMuxer(tmp_pkt);
			muxer->WritePacket(tmp_pkt);

			// Release the packet
			av_packet_unref(tmp_pkt);
			av_packet_free(&tmp_pkt);
		}

		packetQueue.clear();
	}
}
