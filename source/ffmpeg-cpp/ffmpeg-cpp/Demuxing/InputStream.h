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

	protected:

		AVCodecContext* codecContext = nullptr;

		void SetFrameSink(FrameSink* frameSink);

		virtual void ConfigureCodecContext();

	private:

		AVStream* stream;

		FrameSink* output;

		AVFrame* frame;
		
		void CleanUp();
	};


}
