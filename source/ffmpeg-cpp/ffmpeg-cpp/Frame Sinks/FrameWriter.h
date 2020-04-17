#pragma once

#include "ffmpeg-cpp/ffmpeg.h"
#include "ffmpeg-cpp/Demuxing/StreamData.h"

namespace ffmpegcpp
{
	class FrameWriter
	{
	public:

		virtual void WriteFrame(int streamIndex, AVFrame* frame, StreamData* metaData) = 0;

		virtual void Close(int streamIndex) = 0;

		virtual bool IsPrimed() = 0;
	};


}
