#pragma once

#include "ffmpeg.h"
#include "FrameSinks/FrameSink.h"
#include "Sources/InputSource.h"
#include "FFmpegResource.h"

namespace ffmpegcpp
{
	// EncodedFileSource takes a file that is already encoded but not in a container (ie .mp3, .h264)
	// and feeds it to the system.
	class EncodedFileSource : public InputSource
	{

	public:
		EncodedFileSource(const char* inFileName, AVCodecID codecId, FrameSink* output);
		EncodedFileSource(const char* inFileName, const char* codecName, FrameSink* output);
		virtual ~EncodedFileSource();

		void PreparePipeline() override;
		bool IsDone() override;
		void Step() override;

	private:

		void CleanUp();

		bool done = false;

		FrameSink* output;
		
		AVCodecParserContext* parser = nullptr;

		FFmpegResource<AVCodecContext> codecContext;

		int bufferSize;

		FFmpegResource<AVFrame> decoded_frame;
		FFmpegResource<AVPacket> pkt;
		uint8_t* buffer = nullptr;

		FILE* file;

		void Init(const char* inFileName, AVCodec* codec, FrameSink* output);

		void Decode(AVPacket *packet, AVFrame* targetFrame);

		AVRational timeBaseCorrectedByTicksPerFrame;
	};
}
