#include "VideoFilter.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{

	VideoFilter::VideoFilter(const char* filterString, VideoFrameSink* target)
	{
		this->target = target;
		this->filterString = filterString;
	}

	VideoFilter::~VideoFilter()
	{
		CleanUp();
	}

	void VideoFilter::CleanUp()
	{
		avfilter_graph_free(&filter_graph);
		av_frame_free(&filt_frame);
	}

	void VideoFilter::InitDelayed(AVFrame* frame, AVRational* timeBase)
	{
		outputFormat = (AVPixelFormat)frame->format;
		if (outputFormat == AV_PIX_FMT_NONE)
		{
			throw FFmpegException("You must provide a frame sink with a specified pixel format, so that the VideoFilter can convert the frame to that format");
		}

		int ret;
		char args[512];

		const AVFilter *buffersrc = avfilter_get_by_name("buffer");
		const AVFilter *buffersink = avfilter_get_by_name("buffersink");

		filt_frame = av_frame_alloc();
		if (!filt_frame)
		{
			throw FFmpegException("Could not allocate intermediate video frame for filter");
		}

		AVFilterInOut *outputs;
		AVFilterInOut *inputs;

		try
		{
			outputs = avfilter_inout_alloc();
			inputs = avfilter_inout_alloc();

			enum AVPixelFormat out_pix_fmts[] = { outputFormat, AV_PIX_FMT_NONE };

			filter_graph = avfilter_graph_alloc();
			if (!outputs || !inputs || !filter_graph)
			{
				throw FFmpegException("Failed to allocate filter graph");
			}


			/* buffer video source: the decoded frames from the decoder will be inserted here. */
			snprintf(args, sizeof(args),
				"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
				frame->width, frame->height, frame->format,
				timeBase->num, timeBase->den,
				frame->sample_aspect_ratio.num, frame->sample_aspect_ratio.den);
			//codec->codecContext->sample_aspect_ratio.num, codec->codecContext->sample_aspect_ratio.den);

			ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
				args, NULL, filter_graph);
			if (ret < 0)
			{
				throw FFmpegException("Cannot create buffer source", ret);
			}

			/* buffer video sink: to terminate the filter chain. */
			ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
				NULL, NULL, filter_graph);
			if (ret < 0)
			{
				throw FFmpegException("Cannot create buffer sink", ret);
			}

			ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", out_pix_fmts,
				AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
			if (ret < 0)
			{
				throw FFmpegException("Cannot set output pixel format", ret);
			}

			/*
			 * Set the endpoints for the filter graph. The filter_graph will
			 * be linked to the graph described by filters_descr.
			 */

			 /*
			  * The buffer source output must be connected to the input pad of
			  * the first filter described by filters_descr; since the first
			  * filter input label is not specified, it is set to "in" by
			  * default.
			  */
			outputs->name = av_strdup("in");
			outputs->filter_ctx = buffersrc_ctx;
			outputs->pad_idx = 0;
			outputs->next = NULL;

			/*
			 * The buffer sink input must be connected to the output pad of
			 * the last filter described by filters_descr; since the last
			 * filter output label is not specified, it is set to "out" by
			 * default.
			 */
			inputs->name = av_strdup("out");
			inputs->filter_ctx = buffersink_ctx;
			inputs->pad_idx = 0;
			inputs->next = NULL;

			if ((ret = avfilter_graph_parse_ptr(filter_graph, filterString,
				&inputs, &outputs, NULL)) < 0)
			{
				throw FFmpegException("Failed to parse and generate filters", ret);
			}

			if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
			{
				throw FFmpegException("Failed to configure filter graph", ret);
			}

			avfilter_inout_free(&inputs);
			avfilter_inout_free(&outputs);
		}
		catch (FFmpegException e)
		{
			avfilter_inout_free(&inputs);
			avfilter_inout_free(&outputs);
			throw e;
		}
	}

	void VideoFilter::WriteFrame(AVFrame* frame, AVRational* timeBase)
	{
		// lazily initialize because we need the data from the frame to configure our filter graph
		if (!initialized)
		{
			InitDelayed(frame, timeBase);
			initialized = true;
		}

		// this only works because timeBase never changes, otherwise we pass the wrong timeBase to frames later
		this->timeBase = timeBase;

		int ret = av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF);

		PollFilterGraphForFrames();
	}

	void VideoFilter::Close()
	{
		if (!initialized) return; // can't close if we were never opened

		int ret = av_buffersrc_add_frame_flags(buffersrc_ctx, NULL, AV_BUFFERSRC_FLAG_KEEP_REF);
		PollFilterGraphForFrames();

		// close our target as well
		target->Close();

	}

	void VideoFilter::PollFilterGraphForFrames()
	{
		int ret = 0;
		while (ret >= 0)
		{
			ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			{
				return;
			}
			else if (ret < 0)
			{
				throw FFmpegException("Erorr during filtering", ret);
			}

			target->WriteFrame(filt_frame, timeBase);

			av_frame_unref(filt_frame);
		}
	}


	bool VideoFilter::IsPrimed()
	{
		return target->IsPrimed();
	}
}
