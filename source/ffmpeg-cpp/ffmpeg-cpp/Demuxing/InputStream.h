#pragma once

#include "ffmpeg.h"
#include "std.h"
#include "Frame Sinks/FrameSink.h"

namespace ffmpegcpp
{
	class InputStream
	{

	public:

		InputStream(AVStream* stream);
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

		AVStream* stream;

		FrameSinkStream* output;

		AVFrame* frame;
		
		void CleanUp();
	};


}
