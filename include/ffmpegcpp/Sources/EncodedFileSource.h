#pragma once

#include "Sources/InputSource.h"
#include "FFmpegResource.h"

#include <string>

enum AVCodecID;
struct AVCodec;
struct AVCodecContext;
struct AVCodecParserContext;
struct AVFrame;
struct AVPacket;
struct AVRational;

namespace ffmpegcpp
{
	class FrameSink;

	// EncodedFileSource takes a file that is already encoded but not in a container (ie .mp3, .h264)
	// and feeds it to the system.
	class EncodedFileSource : public InputSource
	{

	public:
		EncodedFileSource(const std::string & inFileName, AVCodecID codecId, FrameSink* output);
		EncodedFileSource(const std::string & inFileName, const std::string & codecName, FrameSink* output);
		virtual ~EncodedFileSource();

		void PreparePipeline() override;
		bool IsDone() const override;
		void Step() override;

	private:

		void CleanUp();

		bool done = false;

		FrameSink* output;
		
		FFmpegResource<AVCodecParserContext> parser;

		FFmpegResource<AVCodecContext> codecContext;

		int bufferSize;

		FFmpegResource<AVFrame> decoded_frame;
		FFmpegResource<AVPacket> pkt;
		uint8_t* buffer = nullptr;

		FILE* file;

		void Init(const std::string & inFileName, AVCodec* codec, FrameSink* output);

		void Decode(AVPacket *packet, AVFrame* targetFrame);

		AVRational timeBaseCorrectedByTicksPerFrame;
	};
}
