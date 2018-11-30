#include "VideoEncoder.h"

#include "FFmpegException.h"


namespace ffmpegcpp
{

	VideoEncoder::VideoEncoder(OutputStream* output)
	{
		this->codec = output->GetCodec();
		this->output = output;

		pkt = av_packet_alloc();
		if (!pkt)
		{
			throw FFmpegException("Failed to allocate packet");
		}
	}


	VideoEncoder::~VideoEncoder()
	{
		av_packet_free(&pkt);
	}

	void VideoEncoder::WriteFrame(AVFrame* frame, AVRational* timeBase)
	{
		frame->pts = frameNumber;
		++frameNumber;

		int ret = avcodec_send_frame(codec->GetContext(), frame);
		if (ret < 0)
		{
			throw FFmpegException("Error sending a frame for encoding", ret);
		}

		while (ret >= 0)
		{
			ret = avcodec_receive_packet(codec->GetContext(), pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			{
				return;
			}
			else if (ret < 0)
			{
				throw FFmpegException("Error during encoding", ret);
			}

			//printf("Write packet %3 (size=%5d)\n", data->pkt->pts, data->pkt->size);
			//fwrite(data->pkt->data, 1, data->pkt->size, data->f);
			output->WritePacket(pkt);

			av_packet_unref(pkt);
		}
	}

	AVPixelFormat VideoEncoder::GetRequiredPixelFormat()
	{
		return codec->GetContext()->pix_fmt;
	}

}

