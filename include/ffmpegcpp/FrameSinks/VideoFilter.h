#pragma once

#include "FrameSinks/VideoFrameSink.h"
#include "FFmpegResource.h"

struct AVFilterContext;
struct AVFilterGraph;
struct AVFrame;
struct AVFrame;
struct AVRational;

namespace ffmpegcpp
{
	class VideoFilter : public VideoFrameSink
	{

	public:

		VideoFilter(const char* filterString, VideoFrameSink* target);

		void WriteFrame(AVFrame* frame, AVRational* timeBase) override;
		void Close() override;

		bool IsPrimed() override;

	private:

		void InitDelayed(AVFrame* frame, AVRational* timeBase);
		void PollFilterGraphForFrames();

		VideoFrameSink* target;

		const char* filterString;
		AVPixelFormat outputFormat;

		FFmpegResource<AVFilterGraph> filter_graph;
		AVFilterContext *buffersink_ctx = nullptr;
		AVFilterContext *buffersrc_ctx = nullptr;
		FFmpegResource<AVFrame> filt_frame;

		bool initialized = false;

		AVRational* timeBase;
	};


}
