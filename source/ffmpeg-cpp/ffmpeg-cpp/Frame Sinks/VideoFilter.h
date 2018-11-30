#pragma once

#include "ffmpeg.h"

#include "VideoFrameSink.h"

namespace ffmpegcpp
{
	class VideoFilter : public VideoFrameSink
	{

	public:

		VideoFilter(const char* filterString, VideoFrameSink* target);
		~VideoFilter();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

		virtual AVPixelFormat GetRequiredPixelFormat();

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
