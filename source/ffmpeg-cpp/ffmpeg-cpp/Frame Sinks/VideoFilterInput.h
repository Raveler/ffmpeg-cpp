#pragma once

#include "ffmpeg.h"

namespace ffmpegcpp
{
	class VideoFilterInput
	{

	public:

		VideoFilterInput();
		~VideoFilterInput();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

		bool HasFrame();
		bool IsClosed();
		bool FetchFrame(AVFrame** frame, AVRational** timeBase);
		bool PeekFrame(AVFrame** frame, AVRational** timeBase);

		void Close();


	private:

		AVFifoBuffer *frame_queue;
		AVRational* timeBase;

		bool frameReceived = false;
		bool closed = false;
	};


}
