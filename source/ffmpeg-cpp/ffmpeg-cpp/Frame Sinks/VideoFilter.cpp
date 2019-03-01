#include "VideoFilter.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{

	VideoFilter::VideoFilter(const char* filterString, VideoFrameSink* target)
	{
		this->target = target->CreateStream();
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
		for (int i = 0; i < inputs.size(); ++i)
		{
			delete inputs[i];
		}
		inputs.clear();
	}

	void VideoFilter::ConfigureFilterGraph()
	{
		int ret;
		char args[512];

		const AVFilter *buffersrc = avfilter_get_by_name("buffer");
		const AVFilter *buffersink = avfilter_get_by_name("buffersink");

		filt_frame = av_frame_alloc();
		if (!filt_frame)
		{
			throw FFmpegException("Could not allocate intermediate video frame for filter");
		}

		try
		{
			// create the filter graph
			filter_graph = avfilter_graph_alloc();
			if (!filter_graph)
			{
				throw FFmpegException("Failed to allocate filter graph");
			}

			// create the filter string based on the defined inputs & one output
			string fullFilterString = "";

			// fetch one frame from each input and use it to construct the filter
			AVFrame *frame;
			for (int i = inputs.size()-1; i >= 0; --i)
			{
				if (!inputs[i]->PeekFrame(&frame))
				{
					throw new FFmpegException(string("No frame found for input ") + to_string(i));
				}

				// get the meta data for this input stream
				StreamData* metaData = inputs[i]->GetMetaData();

				/* buffer video source: the decoded frames from the decoder will be inserted here. */
				AVPixelFormat pixelFormat = (AVPixelFormat)frame->format;
				snprintf(args, sizeof(args),
					"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d:frame_rate=%d/%d",
					frame->width, frame->height, frame->format,
					metaData->timeBase.num, metaData->timeBase.den,
					frame->sample_aspect_ratio.num, frame->sample_aspect_ratio.den,
					metaData->frameRate.num, metaData->frameRate.den);

				char bufferString[1000];
				snprintf(bufferString, sizeof(bufferString), "buffer=%s [in_%d]; ", args, i+1);
				fullFilterString = bufferString + fullFilterString; // prepend the buffer string

			}

			// append an output sink to the buffer string
			for (int i = 0; i < inputs.size(); ++i)
			{
				fullFilterString += "[in_" + to_string(i+1) + "] ";
			}
			fullFilterString += filterString;
			fullFilterString += " [result]; [result] buffersink";

			// let avfilter generate the entire filter graph based on this string, including all
			// inputs and outputs. There are other ways to do this, but this is by far the easiest
			// one.
			AVFilterInOut *gis = NULL;
			AVFilterInOut *gos = NULL;
			ret = avfilter_graph_parse2(filter_graph, fullFilterString.c_str(), &gis, &gos);
			if (ret < 0)
			{
				throw FFmpegException("Failed to parse and generate filters", ret);
			}

			// we don't use these
			avfilter_inout_free(&gis);
			avfilter_inout_free(&gos);

			// Fetch all input buffer sources and the output buffer sink from the graph.
			for (int i = 0; i < filter_graph->nb_filters; ++i)
			{
				AVFilterContext* ctx = filter_graph->filters[i];
				if (ctx->nb_inputs == 0)
				{
					bufferSources.push_back(ctx);
				}
				if (ctx->nb_outputs == 0)
				{
					buffersink_ctx = ctx;
				}
			}

			// Finally configure (initialize) the graph.
			if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
			{
				throw FFmpegException("Failed to configure filter graph", ret);
			}

			// we configure our output meta data based on the sink's data
			outputMetaData.timeBase = buffersink_ctx->inputs[0]->time_base;
			outputMetaData.frameRate = buffersink_ctx->inputs[0]->frame_rate;
		}
		catch (FFmpegException e)
		{
			throw e;
		}
	}

	void VideoFilter::DrainInputQueues()
	{
		AVFrame *frame;
		AVRational* timeBase;
		for (int i = 0; i < inputs.size(); ++i)
		{
			while (inputs[i]->FetchFrame(&frame))
			{
				int ret = av_buffersrc_add_frame(bufferSources[i], frame);
				av_frame_free(&frame);
			}
		}
	}

	FrameSinkStream* VideoFilter::CreateStream()
	{

		// each new stream is associated with an input, and we need at least one frame from each input
		// until we can configure the filter graph!
		VideoFilterInput* input = new VideoFilterInput();
		inputs.push_back(input);
		return new FrameSinkStream(this, inputs.size() - 1);
	}

	void VideoFilter::WriteFrame(int streamIndex, AVFrame* frame, StreamData* metaData)
	{
		// lazily initialize because we need the data from the frame to configure our filter graph
		if (!initialized)
		{
			// add to the proper input
			inputs[streamIndex]->SetMetaData(metaData);
			inputs[streamIndex]->WriteFrame(frame);

			// see if all inputs have received a frame - at this point, we can initialize!
			bool allInputsHaveFrames = true;
			for (int i = 0; i < inputs.size(); ++i)
			{
				if (!inputs[i]->HasFrame())
				{
					allInputsHaveFrames = false;
				}
			}

			// all inputs have a frame - we can finally configure!
			if (allInputsHaveFrames)
			{
				ConfigureFilterGraph();
				DrainInputQueues();
				initialized = true;
			}
			
			// we don't do anything else if we haven't finished configuring
			else
			{
				return;
			}
		}

		// add to the proper buffer source
		int ret = av_buffersrc_add_frame_flags(bufferSources[streamIndex], frame, AV_BUFFERSRC_FLAG_KEEP_REF);

		PollFilterGraphForFrames();
	}

	void VideoFilter::Close(int streamIndex)
	{
		if (!initialized) return; // can't close if we were never opened

		int ret = av_buffersrc_add_frame_flags(bufferSources[streamIndex], NULL, AV_BUFFERSRC_FLAG_KEEP_REF);
		PollFilterGraphForFrames();

		// close this input
		inputs[streamIndex]->Close();

		// close our target only if all inputs are closed
		bool allClosed = true;
		for (int i = 0; i < inputs.size(); ++i)
		{
			if (!inputs[i]->IsClosed()) allClosed = false;
		}
		if (allClosed)
		{
			target->Close();
		}
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

			target->WriteFrame(filt_frame, &outputMetaData);

			av_frame_unref(filt_frame);
		}
	}


	bool VideoFilter::IsPrimed()
	{
		return target->IsPrimed();
	}
}
