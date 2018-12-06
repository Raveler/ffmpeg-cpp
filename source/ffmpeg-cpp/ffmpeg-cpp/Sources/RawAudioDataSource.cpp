#include "RawAudioDataSource.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{

	RawAudioDataSource::RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, VideoFrameSink* output)
	{
		this->output = output;

		// create the frame
		int ret;

		frame = av_frame_alloc();
		if (!frame)
		{
			CleanUp();
			throw FFmpegException("Could not allocate video frame");
		}
		/*
		frame->format = sampleFormat;
		frame->sample_rate = sampleRate;
		frame->channels = ;
		frame->channel_layout;
		frame->nb_samples = 1024; // just some amount
		*/
		/* allocate the buffers for the frame data */
		ret = av_frame_get_buffer(frame, 0);
		if (ret < 0)
		{
			CleanUp();
			throw FFmpegException("Could not allocate the video frame data", ret);
		}
	}


	RawAudioDataSource::~RawAudioDataSource()
	{
		CleanUp();
	}

	void RawAudioDataSource::CleanUp()
	{
		if (frame != nullptr)
		{
			av_frame_free(&frame);
			frame = nullptr;
		}
	}

	void RawAudioDataSource::WriteData(void* data, int sampleCount)
	{
		// make sure the frame data is writable
		/*int ret = av_frame_make_writable(frame);
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
		output->WriteFrame(frame, &this->timeBase);*/
	}
}
