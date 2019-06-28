#include "InputStream.h"
#include "CodecDeducer.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	InputStream::InputStream(AVFormatContext* format, AVStream* stream)
	{
		this->stream = stream;
		this->format = format;

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

		codecContext->framerate = stream->avg_frame_rate;

		// Copy codec parameters from input stream to output codec context
		int ret;
		if ((ret = avcodec_parameters_to_context(codecContext, stream->codecpar)) < 0)
		{
			throw FFmpegException("Failed to copy " + string(codec->name) + " codec parameters to decoder context", ret);
		}

		// before we open it, we let our subclasses configure the codec context as well
		ConfigureCodecContext();

		// Init the decoders
		if ((ret = avcodec_open2(codecContext, codec, NULL)) < 0)
		{
			throw FFmpegException("Failed to open codec " + string(codec->name), ret);
		}

		// calculate the "correct" time_base
		// TODO this is definitely an ugly hack but right now I have no idea on how to fix this properly.
		timeBaseCorrectedByTicksPerFrame.num = codecContext->time_base.num;
		timeBaseCorrectedByTicksPerFrame.den = codecContext->time_base.den;
		timeBaseCorrectedByTicksPerFrame.num *= codecContext->ticks_per_frame;

		// assign the frame that will be read from the container
		frame = av_frame_alloc();
		if (!frame)
		{
			throw FFmpegException("Could not allocate frame");
		}
	}

	InputStream::~InputStream()
	{
		CleanUp();
	}

	void InputStream::ConfigureCodecContext()
	{
		// does nothing by default
	}

	void InputStream::Open(FrameSink* frameSink)
	{
		output = frameSink->CreateStream();
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
		if (metaData != nullptr)
		{
			delete metaData;
			metaData = nullptr;
		}
	}

	StreamData* InputStream::DiscoverMetaData()
	{
		/*metaData = new StreamData();

		AVRational* time_base = &timeBaseCorrectedByTicksPerFrame;
		if (!timeBaseCorrectedByTicksPerFrame.num)
		{
			time_base = &stream->time_base;
		}

		metaData->timeBase.num = time_base->num;
		metaData->timeBase.den = time_base->den;*/

		AVRational overrideFrameRate;
		overrideFrameRate.num = 0;

		AVRational tb = overrideFrameRate.num ? av_inv_q(overrideFrameRate) : stream->time_base;
		AVRational fr = overrideFrameRate;
		if (!fr.num) fr = av_guess_frame_rate(format, stream, NULL);

		StreamData* metaData = new StreamData();
		metaData->timeBase = tb;
		metaData->frameRate = fr;

		metaData->type = codecContext->codec->type;

		return metaData;
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

			// put default settings from the stream into the frame
			if (!frame->sample_aspect_ratio.num)
			{
				frame->sample_aspect_ratio = stream->sample_aspect_ratio;
			}

			// the meta data does not exist yet - we figure it out!
			if (metaData == nullptr)
			{
				metaData = DiscoverMetaData();
			}

			// push the frame to the next stage.
			// The time_base is filled in in the codecContext after the first frame is decoded
			// so we can fetch it from there.
			if (output == nullptr)
			{
				// No frame sink specified - just release the frame again.
			}
			else
			{
				output->WriteFrame(frame, metaData);
			}
			++nFramesProcessed;
		}
	}

	int InputStream::GetFramesProcessed()
	{
		return nFramesProcessed;
	}

	void InputStream::Close()
	{
		if (output != nullptr) output->Close();
	}

	bool InputStream::IsPrimed()
	{
		return output->IsPrimed();
	}

	float InputStream::CalculateBitRate(AVCodecContext* ctx)
	{
		int64_t bit_rate;
		int bits_per_sample;

		switch (ctx->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
		case AVMEDIA_TYPE_DATA:
		case AVMEDIA_TYPE_SUBTITLE:
		case AVMEDIA_TYPE_ATTACHMENT:
			bit_rate = ctx->bit_rate;
			break;
		case AVMEDIA_TYPE_AUDIO:
			bits_per_sample = av_get_bits_per_sample(ctx->codec_id);
			bit_rate = bits_per_sample ? ctx->sample_rate * (int64_t)ctx->channels * bits_per_sample : ctx->bit_rate;
			break;
		default:
			bit_rate = 0;
			break;
		}
		return bit_rate / 1000.0f;
	}
}

