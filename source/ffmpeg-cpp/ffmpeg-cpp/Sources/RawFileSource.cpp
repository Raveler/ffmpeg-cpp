#include "RawFileSource.h"
#include "FFmpegException.h"
#include "CodecDeducer.h"

using namespace std;

namespace ffmpegcpp
{
	RawFileSource::RawFileSource(const char* inFileName, const char* codecName, FrameSink* output)
	{
		// deduce the codec from the filename
		AVCodec* codec = CodecDeducer::DeduceDecoder(codecName);
		Init(inFileName, codec, output);
	}

	RawFileSource::RawFileSource(const char* inFileName, AVCodecID codecId, FrameSink* output)
	{
		AVCodec* codec = CodecDeducer::DeduceDecoder(codecId);
		Init(inFileName, codec, output);
	}

	void RawFileSource::Init(const char* inFileName, AVCodec* codec, FrameSink* output)
	{
		this->output = output;
		this->codec = codec;

		parser = av_parser_init(codec->id);
		if (!parser)
		{
			throw FFmpegException("Parser for codec not found " + string(codec->name));
		}

		codecContext = avcodec_alloc_context3(codec);
		if (!codecContext)
		{
			throw FFmpegException("Failed to allocate context for codec " + string(codec->name));
		}

		/* open it */
		if (int ret = avcodec_open2(codecContext, codec, NULL) < 0)
		{
			throw FFmpegException("Failed to open context for codec " + string(codec->name), ret);
		}

		file = fopen(inFileName, "rb");
		if (!file)
		{
			throw FFmpegException("Could not open file " + string(inFileName));
		}
	}

	void RawFileSource::Start()
	{
		uint8_t *data;
		size_t   data_size;
		AVFrame *decoded_frame = NULL;
		int ret;
		AVPacket *pkt;
		uint8_t* buffer;

		decoded_frame = av_frame_alloc();
		if (!decoded_frame)
		{
			throw FFmpegException("Could not allocate video frame");
		}

		// based on the codec, we use different buffer sizes
		int bufferSize, refillThreshold;
		if (codecContext->codec->type == AVMEDIA_TYPE_VIDEO)
		{
			bufferSize = 4096;
			refillThreshold = 0;
		}
		else if (codecContext->codec->type == AVMEDIA_TYPE_AUDIO)
		{
			bufferSize = 20480;
			refillThreshold = 4096;
		}
		else
		{
			throw FFmpegException("Codec " + string(codecContext->codec->name) + " is not supported as a RawFileSource");
		}

		buffer = new uint8_t[bufferSize + AV_INPUT_BUFFER_PADDING_SIZE];

		/* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
		memset(buffer + (int)bufferSize, 0, AV_INPUT_BUFFER_PADDING_SIZE);


		pkt = av_packet_alloc();
		if (!pkt)
		{
			throw FFmpegException("Failed to allocate packet");
		}

		while (!feof(file))
		{
			/* read raw data from the input file */
			data_size = fread(buffer, 1, bufferSize, file);
			if (!data_size)
				break;

			/* use the parser to split the data into frames */
			data = buffer;
			while (data_size > 0)
			{
				ret = av_parser_parse2(parser, codecContext, &pkt->data, &pkt->size,
					data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
				if (ret < 0)
				{
					throw FFmpegException("Error while parsing file", ret);
				}
				data += ret;
				data_size -= ret;

				if (pkt->size)
				{
					Decode(pkt, decoded_frame);
				}

				/*if (data_size < refillThreshold)
				{
					memmove(buffer, data, data_size);
					data = buffer;
					int len = fread(data + data_size, 1,
						bufferSize - data_size, file);
					if (len > 0) data_size += len;
				}*/
			}
		}

		/* flush the decoder */
		pkt->data = NULL;
		pkt->size = 0;
		Decode(pkt, decoded_frame);

		av_frame_free(&decoded_frame);
		av_packet_free(&pkt);

		delete buffer;
	}

	void RawFileSource::Decode(AVPacket *pkt, AVFrame *frame)
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
			/*data_size = av_get_bytes_per_sample(codecContext->sample_fmt);
			if (data_size < 0)
			{
				// This should not occur, checking just for paranoia
				throw FFmpegException("Failed to calculate data size");
			}*/

			// push the frame to the next stage.
			// The time_base is filled in in the codecContext after the first frame is decoded
			// so we can fetch it from there.
			output->WriteFrame(frame, &codecContext->time_base);
		}
	}

	RawFileSource::~RawFileSource()
	{
		fclose(file);

		avcodec_free_context(&codecContext);
		av_parser_close(parser);
	}
}
