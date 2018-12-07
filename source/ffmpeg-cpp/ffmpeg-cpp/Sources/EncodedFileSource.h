#pragma once

#include "ffmpeg.h"
#include "Frame Sinks/FrameSink.h"
#include "InputSource.h"

namespace ffmpegcpp
{
	// EncodedFileSource takes a file that is already encoded but not in a container (ie .mp3, .h264)
	// and feeds it to the system.
	class EncodedFileSource : public InputSource
	{

	public:
		EncodedFileSource(const char* inFileName, AVCodecID codecId, FrameSink* output);
		EncodedFileSource(const char* inFileName, const char* codecName, FrameSink* output);
		~EncodedFileSource();

		void Start();


	private:

		FrameSink* output;
		
		AVCodecParserContext* parser;

		AVCodec* codec;
		AVCodecContext* codecContext;

		FILE* file;

		void Init(const char* inFileName, AVCodec* codec, FrameSink* output);

		void Decode(AVPacket *packet, AVFrame* targetFrame);

		AVRational timeBaseCorrectedByTicksPerFrame;
	};
}
