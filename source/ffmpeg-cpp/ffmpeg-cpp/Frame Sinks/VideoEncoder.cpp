#include "VideoEncoder.h"

#include "FFmpegException.h"
#include "Muxing/VideoOutputStream.h"

using namespace std;

namespace ffmpegcpp
{

	VideoEncoder::VideoEncoder(VideoCodec* codec, Muxer* muxer)
	{
		this->closedCodec = codec;

		// create an output stream
		this->output = new VideoOutputStream(muxer, codec);
		muxer->AddOutputStream(output);

		// this one is used to make sure we only allow one frame sink stream to be generated
		oneInputFrameSink = new OneInputFrameSink(this, AVMEDIA_TYPE_VIDEO);
	}

	VideoEncoder::VideoEncoder(VideoCodec* codec, Muxer* muxer, AVPixelFormat format)
		: VideoEncoder(codec, muxer)
	{
		finalPixelFormat = format;
	}

	VideoEncoder::VideoEncoder(VideoCodec* codec, Muxer* muxer, AVRational frameRate)
		: VideoEncoder(codec, muxer)
	{
		finalFrameRate = frameRate;
		finalFrameRateSet = true;
	}

	VideoEncoder::VideoEncoder(VideoCodec* codec, Muxer* muxer, AVRational frameRate, AVPixelFormat format)
		: VideoEncoder(codec, muxer)
	{
		finalPixelFormat = format;
		finalFrameRate = frameRate;
		finalFrameRateSet = true;
	}

	VideoEncoder::~VideoEncoder()
	{
		CleanUp();
	}

	void VideoEncoder::CleanUp()
	{
		if (pkt != nullptr)
		{
			av_packet_free(&pkt);
		}
		if (codec != nullptr)
		{
			delete codec;
			codec = nullptr;
		}
		if (formatConverter != nullptr)
		{
			delete formatConverter;
			formatConverter = nullptr;
		}
		if (oneInputFrameSink != nullptr)
		{
			delete oneInputFrameSink;
			oneInputFrameSink = nullptr;
		}
	}

	void VideoEncoder::OpenLazily(AVFrame* frame, StreamData* metaData)
	{
		// configure the parameters for the codec based on the frame and our preferences
		int width = frame->width;
		int height = frame->height;

		// the format is derived in this order:
		// 1. The provided format if it was specified and supported by the codec
		// 2. Otherwise, the format of the input frame if it is supported by the codec
		// 3. Otherwise, the default format of the codec
		// the format is either the provided format, or the default format if it is not supported
		AVPixelFormat format = finalPixelFormat;
		if (format == AV_PIX_FMT_NONE) format = (AVPixelFormat)frame->format;
		if (!closedCodec->IsPixelFormatSupported(format)) format = closedCodec->GetDefaultPixelFormat();

		// the frame rate is either the input frame rate, OR the default frame rate if the input frame rate
		// is not supported, OR the explicitly chosen framerate.
		AVRational frameRate = metaData->frameRate;
		if (!closedCodec->IsFrameRateSupported(&frameRate)) frameRate = closedCodec->GetClosestSupportedFrameRate(frameRate);
		if (finalFrameRateSet) frameRate = finalFrameRate;

		// open the codec
		codec = closedCodec->Open(width, height, &frameRate, format);

		// allocate the packet we'll be using
		pkt = av_packet_alloc();
		if (!pkt)
		{
			CleanUp();
			throw FFmpegException("Failed to allocate packet");
		}

		// set up the format converter
		try
		{
			formatConverter = new VideoFormatConverter(codec->GetContext());
		}
		catch (FFmpegException e)
		{
			CleanUp();
			throw e;
		}
	}

	FrameSinkStream* VideoEncoder::CreateStream()
	{
		return oneInputFrameSink->CreateStream();
	}

	void VideoEncoder::WriteFrame(int streamIndex, AVFrame* frame, StreamData* metaData)
	{
		// if we haven't opened the codec yet, we do it now!
		if (codec == nullptr)
		{
			OpenLazily(frame, metaData);
		}

		// convert the frame to a converted_frame
		frame = formatConverter->ConvertFrame(frame);

		if (frame->format != codec->GetContext()->pix_fmt)
		{
			throw FFmpegException("Codec only accepts " + string(av_get_pix_fmt_name(codec->GetContext()->pix_fmt)) + " while frame is in format " + av_get_pix_fmt_name((AVPixelFormat)frame->format));
		}

		frame->pts = frameNumber;
		++frameNumber;

		int ret = avcodec_send_frame(codec->GetContext(), frame);
		if (ret < 0)
		{
			throw FFmpegException("Error sending a frame for encoding", ret);
		}

		PollCodecForPackets();
	}

	void VideoEncoder::Close(int streamIndex)
	{
		if (codec == nullptr) return; // can't close if we were never opened

		int ret = avcodec_send_frame(codec->GetContext(), NULL);
		if (ret < 0)
		{
			throw FFmpegException("Error flushing codec after encoding", ret);
		}

		PollCodecForPackets();
	}

	void VideoEncoder::PollCodecForPackets()
	{
		int ret = 0;
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
			output->WritePacket(pkt, codec);

			av_packet_unref(pkt);
		}
	}

	bool VideoEncoder::IsPrimed()
	{
		return output->IsPrimed();
	}
}

