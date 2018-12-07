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

	private:

		void InitDelayed(AVFrame* frame, AVRational* timeBase);

		VideoFrameSink* target;

		const char* filterString;
		AVPixelFormat outputFormat;

		AVFilterGraph *filter_graph = nullptr;
		AVFilterContext *buffersink_ctx = nullptr;
		AVFilterContext *buffersrc_ctx = nullptr;
		AVFrame* filt_frame = nullptr;

		bool initialized = false;

		void CleanUp();
	};


}
