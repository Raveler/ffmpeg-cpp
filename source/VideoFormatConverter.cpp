#include "VideoFormatConverter.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{
	VideoFormatConverter::VideoFormatConverter(AVCodecContext* codecContext)
	{
		this->codecContext = codecContext;

		converted_frame = av_frame_alloc();
		int ret;
		if (!converted_frame)
		{
			CleanUp();
			throw FFmpegException("Error allocating a video frame");
		}

		// configure the frame and get the buffer
		converted_frame->format = codecContext->pix_fmt;
		converted_frame->width = codecContext->width;
		converted_frame->height = codecContext->height;

		/* allocate the buffers for the frame data */
		ret = av_frame_get_buffer(converted_frame, 32);
		if (ret < 0)
		{
			CleanUp();
			throw FFmpegException("Failed to allocate buffer for frame", ret);
		}
	}

	VideoFormatConverter::~VideoFormatConverter()
	{
		CleanUp();
	}

	void VideoFormatConverter::CleanUp()
	{
		if (converted_frame != nullptr)
		{
			av_frame_free(&converted_frame);
			converted_frame = nullptr;
		}
		if (swsContext != nullptr)
		{
			sws_freeContext(swsContext);
			swsContext = nullptr;
		}
	}

	void VideoFormatConverter::InitDelayed(AVFrame* frame)
	{
		// configure the conversion context based in the source and target data
		swsContext = sws_getCachedContext(swsContext,
			frame->width, frame->height, (AVPixelFormat)frame->format,
			converted_frame->width, converted_frame->height, (AVPixelFormat)converted_frame->format,
			0, 0, 0, 0);

	}

	AVFrame* VideoFormatConverter::ConvertFrame(AVFrame* frame)
	{
		// initialize the resampler
		if (!initialized)
		{
			InitDelayed(frame);
			initialized = true;
		}

		// convert the frame
		sws_scale(swsContext, frame->data, frame->linesize, 0,
			frame->height, converted_frame->data, converted_frame->linesize);

		av_frame_copy_props(converted_frame, frame); // remember all the other data

		return converted_frame;
	}
}

