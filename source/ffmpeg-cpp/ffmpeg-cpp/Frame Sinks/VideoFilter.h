#pragma once

#include "ffmpeg.h"

#include "FrameSink.h"

namespace ffmpegcpp
{
	class VideoFilter : public FrameSink
	{

	public:

		VideoFilter(const char* filterString, AVPixelFormat outputPixelFormat, FrameSink* target);
		~VideoFilter();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

	private:

		void InitDelayed(AVFrame* frame, AVRational* timeBase);

		FrameSink* target;

		const char* filterString;
		AVPixelFormat outputFormat;

		AVFilterGraph *filter_graph;
		AVFilterContext *buffersink_ctx;
		AVFilterContext *buffersrc_ctx;
		AVFrame* filt_frame;

		bool initialized = false;
	};


}
