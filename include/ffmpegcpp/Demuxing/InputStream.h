#pragma once

#include "ffmpeg.h"
#include "std.h"
#include "FrameSinks/FrameSink.h"
#include "FFmpegResource.h"

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

		FFmpegResource<AVCodecContext> codecContext;

		void SetFrameSink(FrameSink* frameSink);

		virtual void ConfigureCodecContext();

	private:

		AVRational timeBaseCorrectedByTicksPerFrame;

		AVStream* stream;

		FrameSink* output;

		AVFrame* frame;
		
		void CleanUp();
	};


}
