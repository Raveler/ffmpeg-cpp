#pragma once

#include "ffmpeg-cpp/ffmpeg.h"
#include "FrameWriter.h"
#include "ffmpeg-cpp/Demuxing/StreamData.h"

namespace ffmpegcpp
{
	class FrameSinkStream
	{
	public:

		FrameSinkStream(FrameWriter* frameSink, int streamIdx);

		void WriteFrame(AVFrame* frame, StreamData* metaData);

		void Close();

		bool IsPrimed();

	private:

		FrameWriter* frameSink;
		int streamIndex;
	};
}
