#include "RawVideoSource.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{

	RawVideoSource::RawVideoSource(int width, int height, AVPixelFormat pixelFormat, int framesPerSecond, FrameSink* output)
		: RawVideoSource(width, height, pixelFormat, pixelFormat, framesPerSecond, output)
	{

	}

	RawVideoSource::RawVideoSource(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, FrameSink* output)
	{
		Init(width, height, sourcePixelFormat, targetPixelFormat, framesPerSecond, output);
	}

	void RawVideoSource::Init(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, FrameSink* output)
	{
		this->output = output;
		this->sourcePixelFormat = sourcePixelFormat;

		// set up the time base
		this->timeBase.num = 1;
		this->timeBase.den = framesPerSecond;

		// create the frame
		int ret;

		frame = av_frame_alloc();
		if (!frame)
		{
			throw FFmpegException("Could not allocate video frame");
		}

		frame->format = targetPixelFormat;
		frame->width = width;
		frame->height = height;

		/* allocate the buffers for the frame data */
		ret = av_frame_get_buffer(frame, 32);
		if (ret < 0)
		{
			throw FFmpegException("Could not allocate the video frame data", ret);
		}
	}

	RawVideoSource::~RawVideoSource()
	{
		av_frame_free(&frame);
	}

	void RawVideoSource::WriteFrame(void* data, int bytesPerRow)
	{
		// make sure the frame data is writable
		int ret = av_frame_make_writable(frame);
		if (ret < 0)
		{
			throw FFmpegException("Error making frame writable", ret);
		}

		const int in_linesize[1] = { bytesPerRow };

		// if the source and target pixel format are the same, we don't do any conversions, we just copy
		// but we use sws_scale anyway because we need to convert to the internal line_size format of frame
		swsContext = sws_getCachedContext(swsContext,
			frame->width, frame->height, sourcePixelFormat,
			frame->width, frame->height, (AVPixelFormat)frame->format,
			0, 0, 0, 0);
		sws_scale(swsContext, (const uint8_t * const *)&data, in_linesize, 0,
			frame->height, frame->data, frame->linesize);

		// send to the output
		output->WriteFrame(frame, &this->timeBase);
	}
}
