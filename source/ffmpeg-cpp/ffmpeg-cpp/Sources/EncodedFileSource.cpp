#include "EncodedFileSource.h"
#include "FFmpegException.h"
#include "CodecDeducer.h"

using namespace std;

namespace ffmpegcpp
{
	EncodedFileSource::EncodedFileSource(const char* inFileName, const char* codecName, FrameSink* output)
	{
		try
		{
			AVCodec* codec = CodecDeducer::DeduceDecoder(codecName);
			Init(inFileName, codec, output);
		}
		catch (FFmpegException e)
		{
			CleanUp();
			throw e;
		}
	}

	EncodedFileSource::EncodedFileSource(const char* inFileName, AVCodecID codecId, FrameSink* output)
	{
		try
		{
			AVCodec* codec = CodecDeducer::DeduceDecoder(codecId);
			Init(inFileName, codec, output);
		}
		catch (FFmpegException e)
		{
			CleanUp();
			throw e;
		}
	}

	EncodedFileSource::~EncodedFileSource()
	{
		CleanUp();
	}

	void EncodedFileSource::CleanUp()
	{
		if (decoded_frame != nullptr)
		{
			av_frame_free(&decoded_frame);
			decoded_frame = nullptr;
		}
		if (pkt != nullptr)
		{
			av_packet_free(&pkt);
			pkt = nullptr;
		}
		if (buffer != nullptr)
		{
			delete buffer;
			buffer = nullptr;
		}
		if (codecContext != nullptr)
		{
			avcodec_free_context(&codecContext);
			codecContext = nullptr;
		}
		if (parser != nullptr)
		{
			av_parser_close(parser);
			parser = nullptr;
		}
		if (metaData != nullptr)
		{
			delete metaData;
			metaData = nullptr;
		}

		fclose(file);
	}

	void EncodedFileSource::Init(const char* inFileName, AVCodec* codec, FrameSink* output)
	{
		this->output = output->CreateStream();
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

		decoded_frame = av_frame_alloc();
		if (!decoded_frame)
		{
			throw FFmpegException("Could not allocate video frame");
		}

		pkt = av_packet_alloc();
		if (!pkt)
		{
			throw FFmpegException("Failed to allocate packet");
		}

		// based on the codec, we use different buffer sizes
		int refillThreshold;
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
	}

	void EncodedFileSource::PreparePipeline()
	{
		while (!output->IsPrimed() && !IsDone())
		{
			Step();
		}
	}

	bool EncodedFileSource::IsDone()
	{
		return done;
	}

	void EncodedFileSource::Step()
	{
		// one step is one part of a buffer read, this might contain no, one or multiple packets

		uint8_t *data;
		size_t   data_size;
		int ret;

		/* read raw data from the input file */
		data_size = fread(buffer, 1, bufferSize, file);
		if (!data_size)	return;

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
		}

		// reached the end of the file - flush everything
		if (feof(file))
		{

			/* flush the decoder */
			pkt->data = NULL;
			pkt->size = 0;
			Decode(pkt, decoded_frame);

			output->Close();

			done = true;
		}
	}

	void EncodedFileSource::Decode(AVPacket *pkt, AVFrame *frame)
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

			if (metaData == nullptr)
			{
				// calculate the "correct" time_base
				// TODO this is definitely an ugly hack but right now I have no idea on how to fix this properly.
				timeBaseCorrectedByTicksPerFrame.num = codecContext->time_base.num;
				timeBaseCorrectedByTicksPerFrame.den = codecContext->time_base.den;
				timeBaseCorrectedByTicksPerFrame.num *= codecContext->ticks_per_frame;


				metaData = new StreamData();
				metaData->timeBase.num = timeBaseCorrectedByTicksPerFrame.num;
				metaData->timeBase.den = timeBaseCorrectedByTicksPerFrame.den;
				metaData->frameRate.den = timeBaseCorrectedByTicksPerFrame.num;
				metaData->frameRate.num = timeBaseCorrectedByTicksPerFrame.den;
			}


			// push the frame to the next stage.
			// The time_base is filled in in the codecContext after the first frame is decoded
			// so we can fetch it from there.
			output->WriteFrame(frame, metaData);
		}
	}
}
