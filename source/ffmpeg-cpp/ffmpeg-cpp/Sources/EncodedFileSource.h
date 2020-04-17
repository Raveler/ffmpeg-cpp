#pragma once

#include "ffmpeg-cpp/ffmpeg.h"
#include "ffmpeg-cpp/Frame Sinks/FrameSink.h"
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
		virtual ~EncodedFileSource();

		virtual void PreparePipeline();
		virtual bool IsDone();
		virtual void Step();

	private:

		void CleanUp();

		bool done = false;

		FrameSinkStream* output;
		
		AVCodecParserContext* parser = nullptr;

		AVCodec* codec;
		AVCodecContext* codecContext = nullptr;

		int bufferSize;

		AVFrame* decoded_frame = nullptr;
		AVPacket* pkt = nullptr;
		uint8_t* buffer = nullptr;

		FILE* file;

		void Init(const char* inFileName, AVCodec* codec, FrameSink* output);

		void Decode(AVPacket *packet, AVFrame* targetFrame);

		AVRational timeBaseCorrectedByTicksPerFrame;

		StreamData* metaData = nullptr;
	};
}
