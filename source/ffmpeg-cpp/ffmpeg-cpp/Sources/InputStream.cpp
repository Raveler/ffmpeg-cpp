#include "InputStream.h"
#include "CodecDeducer.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	InputStream::InputStream(AVStream* stream)
	{
		// find decoder for the stream
		AVCodec* codec = CodecDeducer::DeduceDecoder(stream->codecpar->codec_id);
		if (!codec)
		{
			CleanUp();
			throw FFmpegException("Failed to find codec for stream " + to_string(stream->index));
		}

		// Allocate a codec context for the decoder
		codecContext = avcodec_alloc_context3(codec);
		if (!codecContext)
		{
			CleanUp();
			throw FFmpegException("Failed to allocate the codec context for " + string(codec->name));
		}

		// Copy codec parameters from input stream to output codec context
		int ret;
		if ((ret = avcodec_parameters_to_context(codecContext, stream->codecpar)) < 0)
		{
			CleanUp();
			throw FFmpegException("Failed to copy " + string(codec->name) + " codec parameters to decoder context", ret);
		}

		// Init the decoders
		if ((ret = avcodec_open2(codecContext, codec, NULL)) < 0)
		{
			CleanUp();
			throw FFmpegException("Failed to open codec " + string(codec->name), ret);
		}

		// assign the frame that will be read from the container
		frame = av_frame_alloc();
		if (!frame)
		{
			CleanUp();
			throw FFmpegException("Could not allocate frame");
		}
	}

	InputStream::~InputStream()
	{
		CleanUp();
	}

	void InputStream::CleanUp()
	{
		if (codecContext != nullptr)
		{
			avcodec_free_context(&codecContext);
			codecContext = nullptr;
		}
		if (frame != nullptr)
		{
			av_frame_free(&frame);
			frame = nullptr;
		}
	}

	void InputStream::SetFrameSink(FrameSink* frameSink)
	{
		output = frameSink;
	}

	void InputStream::DecodePacket(AVPacket *pkt)
	{
		int ret;

		/* send the packet with the compressed data to the decoder */
		ret = avcodec_send_packet(codecContext, pkt);
		if (ret < 0)
		{
			throw FFmpegException("Error submitting the packet to the decoder", ret);
		}

		/* read all the output frames (in general there may be any number of them */
		while (ret >= 0)
		{
			ret = avcodec_receive_frame(codecContext, frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return;
			else if (ret < 0)
			{
				throw FFmpegException("Error during decoding", ret);
			}

			// push the frame to the next stage.
			// The time_base is filled in in the codecContext after the first frame is decoded
			// so we can fetch it from there.
			output->WriteFrame(frame, &codecContext->time_base);
		}
	}
}

