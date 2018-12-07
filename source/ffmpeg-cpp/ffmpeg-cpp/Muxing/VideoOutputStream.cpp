#include "VideoOutputStream.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	VideoOutputStream::VideoOutputStream(Muxer* muxer, Codec* codec)
		: OutputStream(muxer, codec)
	{
	}

	void VideoOutputStream::OpenStream(AVStream* stream, int containerFlags)
	{
		this->stream = stream;

		// special case for certain containers
		if (containerFlags & AVFMT_GLOBALHEADER)
		{
			codec->SetGlobalContainerHeader();
		}
	}

	void VideoOutputStream::LazilyInitialize(OpenCodec* openCodec)
	{

		stream->time_base = openCodec->GetContext()->time_base;
		stream->avg_frame_rate = openCodec->GetContext()->framerate;

		stream->disposition = 1;

		/* copy the stream parameters to the muxer */
		int ret = avcodec_parameters_from_context(stream->codecpar, openCodec->GetContext());
		if (ret < 0)
		{
			throw FFmpegException("Could not copy codec parameters to stream", ret);
		}
	}

	void VideoOutputStream::WritePacket(AVPacket *pkt, OpenCodec* openCodec)
	{
		if (!initialized)
		{
			LazilyInitialize(openCodec);
			initialized = true;
		}

		/* rescale output packet timestamp values from codec to stream timebase */
		AVRational* time_base = &openCodec->GetContext()->time_base;
		av_packet_rescale_ts(pkt, *time_base, stream->time_base);
		pkt->stream_index = stream->index;

		// We NEED to fill in the duration here, otherwise the frame rate is calculated wrong in the end for certain codecs/containers (ie h264/mp4).
		pkt->duration = stream->time_base.den / stream->time_base.num / stream->avg_frame_rate.num * stream->avg_frame_rate.den;

		/* Write the compressed frame to the media file. */
		muxer->WritePacket(pkt);
	}
}
