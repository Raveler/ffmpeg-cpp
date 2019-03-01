#include "RawVideoDataSource.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{

	RawVideoDataSource::RawVideoDataSource(int width, int height, AVPixelFormat pixelFormat, int framesPerSecond, VideoFrameSink* output)
		: RawVideoDataSource(width, height, pixelFormat, pixelFormat, framesPerSecond, output)
	{

	}

	RawVideoDataSource::RawVideoDataSource(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, VideoFrameSink* output)
	{
		Init(width, height, sourcePixelFormat, targetPixelFormat, framesPerSecond, output);
	}

	void RawVideoDataSource::Init(int width, int height, AVPixelFormat sourcePixelFormat, AVPixelFormat targetPixelFormat, int framesPerSecond, VideoFrameSink* output)
	{
		this->output = output->CreateStream();
		this->sourcePixelFormat = sourcePixelFormat;

		// set up the time base
		metaData.timeBase.num = 1;
		metaData.timeBase.den = framesPerSecond;
		metaData.frameRate.num = framesPerSecond;
		metaData.frameRate.den = 1;

		// create the frame
		int ret;

		frame = av_frame_alloc();
		if (!frame)
		{
			CleanUp();
			throw FFmpegException("Could not allocate video frame");
		}

		frame->format = targetPixelFormat;
		frame->width = width;
		frame->height = height;

		/* allocate the buffers for the frame data */
		ret = av_frame_get_buffer(frame, 32);
		if (ret < 0)
		{
			CleanUp();
			throw FFmpegException("Could not allocate the video frame data", ret);
		}
	}

	RawVideoDataSource::~RawVideoDataSource()
	{
		CleanUp();
	}

	void RawVideoDataSource::CleanUp()
	{
		if (frame != nullptr)
		{
			av_frame_free(&frame);
			frame = nullptr;
		}
		if (swsContext != nullptr)
		{
			sws_freeContext(swsContext);
			swsContext = nullptr;
		}
	}

	void RawVideoDataSource::WriteFrame(void* data, int bytesPerRow)
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
		output->WriteFrame(frame, &metaData);
	}

	void RawVideoDataSource::Close()
	{
		output->Close();
	}

	int RawVideoDataSource::GetWidth()
	{
		return frame->width;
	}

	int RawVideoDataSource::GetHeight()
	{
		return frame->height;
	}

	bool RawVideoDataSource::IsPrimed()
	{
		return output->IsPrimed();
	}
}
