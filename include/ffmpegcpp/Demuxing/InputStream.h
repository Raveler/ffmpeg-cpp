#pragma once

#include "FFmpegResource.h"

struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct AVRational;
struct AVStream;

namespace ffmpegcpp
{
	class FrameSink;

	class InputStream
	{

	public:

		InputStream(AVStream* stream);

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

		FFmpegResource<AVFrame> frame;
	};


}
