#pragma once

#include "ffmpeg.h"
#include "OutputStream.h"
#include "Frame Sinks/FrameSink.h"

namespace ffmpegcpp
{
	class RawFileSource
	{

	public:
		RawFileSource(const char* inFileName, AVCodecID codecId, FrameSink* output);
		RawFileSource(const char* inFileName, const char* codecName, FrameSink* output);
		~RawFileSource();

		void Start();


	private:

		FrameSink* output;
		
		AVCodecParserContext* parser;

		AVCodec* codec;
		AVCodecContext* codecContext;

		FILE* file;

		void Init(const char* inFileName, AVCodec* codec, FrameSink* output);

		void Decode(AVPacket *packet, AVFrame* targetFrame);
	};
}
