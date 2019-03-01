#include "VideoFilterInput.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{
	VideoFilterInput::VideoFilterInput()
	{

		// create the queue that will be used to store all the frames until they are drained
		frame_queue = av_fifo_alloc(8 * sizeof(AVFrame*));
	}

	VideoFilterInput::~VideoFilterInput()
	{
		av_fifo_freep(&frame_queue);
	}

	void VideoFilterInput::WriteFrame(AVFrame* frame)
	{
		AVFrame *tmp = av_frame_clone(frame);
		if (!tmp) throw new FFmpegException("Failed to clone frame");
		av_frame_unref(frame);

		// store the frame into a fifo queue
		if (!av_fifo_space(frame_queue))
		{
			int ret = av_fifo_realloc2(frame_queue, 2 * av_fifo_size(frame_queue));
			if (ret < 0)
			{
				av_frame_free(&tmp);
				throw new FFmpegException("Failed to allocate buffer for fifo queue", ret);
			}
		}

		av_fifo_generic_write(frame_queue, &tmp, sizeof(tmp), NULL);
		frameReceived = true;
	}

	bool VideoFilterInput::HasFrame()
	{
		return frameReceived;
	}

	bool VideoFilterInput::FetchFrame(AVFrame** frame)
	{
		// see if there's anything in the queue
		int framesInQueue = av_fifo_size(frame_queue);
		if (framesInQueue == 0) return false;

		// fetch the frame from the queue
		AVFrame *tmp;
		av_fifo_generic_read(frame_queue, &tmp, sizeof(tmp), NULL);

		*frame = tmp;

		return true;
	}

	bool VideoFilterInput::PeekFrame(AVFrame** frame)
	{
		// see if there's anything in the queue
		int framesInQueue = av_fifo_size(frame_queue);
		if (framesInQueue == 0) return false;

		// fetch the frame from the queue
		AVFrame *tmp;
		av_fifo_generic_peek(frame_queue, &tmp, sizeof(tmp), NULL);

		*frame = tmp;

		return true;
	}

	void VideoFilterInput::SetMetaData(StreamData* metaData)
	{
		this->metaData = metaData;
	}

	StreamData* VideoFilterInput::GetMetaData()
	{
		return metaData;
	}

	void VideoFilterInput::Close()
	{
		closed = true;
	}

	bool VideoFilterInput::IsClosed()
	{
		return closed;
	}
}
