#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "VideoFrameSink.h"
#include "VideoFilterInput.h"

namespace ffmpegcpp
{
	class VideoFilter : public VideoFrameSink, public FrameWriter
	{

	public:

		VideoFilter(const char* filterString, VideoFrameSink* target);
		virtual ~VideoFilter();

		FrameSinkStream* CreateStream();

		void WriteFrame(int streamIndex, AVFrame* frame, StreamData* metaData);
		void Close(int streamIndex);


		bool IsPrimed();

	private:

		void ConfigureFilterGraph();
		void DrainInputQueues();
		void PollFilterGraphForFrames();

		std::vector<VideoFilterInput*> inputs;
		std::vector<AVFilterContext*> bufferSources;

		FrameSinkStream* target;

		const char* filterString;

		AVFilterGraph *filter_graph = nullptr;
		AVFilterContext *buffersink_ctx = nullptr;
		AVFrame* filt_frame = nullptr;

		bool initialized = false;

		StreamData outputMetaData;

		void CleanUp();
	};


}
