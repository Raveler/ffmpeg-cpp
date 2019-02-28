#pragma once

#include "ffmpeg.h"
#include "std.h"
#include "Frame Sinks/FrameSink.h"

namespace ffmpegcpp
{
	class InputStream
	{

	public:

		InputStream(AVFormatContext* format, AVStream* stream);
		~InputStream();

		void Open();

		virtual void DecodePacket(AVPacket* pkt);
		void Close();

		bool IsPrimed();

	protected:

		AVCodecContext* codecContext = nullptr;

		void SetFrameSink(FrameSink* frameSink);

		virtual void ConfigureCodecContext();

	private:

		AVRational timeBaseCorrectedByTicksPerFrame;

		AVFormatContext* format;
		AVStream* stream;

		FrameSinkStream* output;

		AVFrame* frame;

		StreamData* metaData = nullptr;

		StreamData* DiscoverMetaData();
		
		void CleanUp();
	};


}
