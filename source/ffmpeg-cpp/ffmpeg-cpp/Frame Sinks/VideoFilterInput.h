#pragma once

#include "ffmpeg.h"
#include "Demuxing/StreamData.h"

namespace ffmpegcpp
{
	class VideoFilterInput
	{

	public:

		VideoFilterInput();
		~VideoFilterInput();

		void WriteFrame(AVFrame* frame);

		bool HasFrame();
		bool IsClosed();
		bool FetchFrame(AVFrame** frame);
		bool PeekFrame(AVFrame** frame);

		void SetMetaData(StreamData* metaData);
		StreamData* GetMetaData();

		void Close();


	private:

		AVFifoBuffer *frame_queue;
		StreamData* metaData;

		bool frameReceived = false;
		bool closed = false;
	};


}
