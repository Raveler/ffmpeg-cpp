#include "Muxing/AudioOutputStream.h"
#include "FFmpegException.h"

using namespace std;
namespace ffmpegcpp
{
	AudioOutputStream::AudioOutputStream(Muxer* muxer, Codec* codec)
		: OutputStream(muxer, codec)
	{
	}

	void AudioOutputStream::OpenStream(AVStream* stream, int containerFlags)
	{
		this->stream = stream;

		// special case for certain containers
		if (containerFlags & AVFMT_GLOBALHEADER)
		{
			codec->SetGlobalContainerHeader();
		}

	}

	void AudioOutputStream::LazilyInitialize(OpenCodec* openCodec)
	{
		// set the time base to the sample rate
		AVRational time_base;
		time_base.num = 1;
		time_base.den = openCodec->GetContext()->sample_rate;
		stream->time_base = time_base;

		/* copy the stream parameters to the muxer */
		int ret = avcodec_parameters_from_context(stream->codecpar, openCodec->GetContext());
		if (ret < 0)
		{
			throw FFmpegException("Could not copy codec parameters to stream", ret);
		}

		codecTimeBase = openCodec->GetContext()->time_base;

		// Copy side_data from the codec context to the stream... this is 
		// necessary for certain codecs such as mpeg2video!
		if (openCodec->GetContext()->nb_coded_side_data)
		{
			int i;

			for (i = 0; i < openCodec->GetContext()->nb_coded_side_data; i++)
			{
				const AVPacketSideData *sd_src = &openCodec->GetContext()->coded_side_data[i];
				uint8_t *dst_data;

				dst_data = av_stream_new_side_data(stream, sd_src->type, sd_src->size);
				if (!dst_data)
				{
					throw FFmpegException("Failed to allocate memory for new side_data");
				}
				memcpy(dst_data, sd_src->data, sd_src->size);
			}
		}
	}

	void AudioOutputStream::WritePacket(AVPacket *pkt, OpenCodec* openCodec)
	{
		if (!initialized)
		{
			LazilyInitialize(openCodec);
			initialized = true;
		}

		// Write the compressed frame to the media file.
		SendPacketToMuxer(pkt);
	}

	void AudioOutputStream::PreparePacketForMuxer(AVPacket* pkt)
	{
		/* rescale output packet timestamp values from codec to stream timebase */
		AVRational* time_base = &codecTimeBase;
		av_packet_rescale_ts(pkt, *time_base, stream->time_base);
		pkt->stream_index = stream->index;

		// We NEED to fill in the duration here, otherwise the frame rate is calculated wrong in the end for certain codecs/containers (ie h264/mp4).
		if (stream->time_base.num != 0 && stream->avg_frame_rate.num != 0)
		{
			pkt->duration = stream->time_base.den / stream->time_base.num / stream->avg_frame_rate.num * stream->avg_frame_rate.den;
		}
	}

	bool AudioOutputStream::IsPrimed()
	{
		return initialized;
	}
}
