#include "FrameSinkStream.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{

	FrameSinkStream::FrameSinkStream(FrameWriter* frameSink, int streamIndex)
	{
		this->frameSink = frameSink;
		this->streamIndex = streamIndex;
	}


	void FrameSinkStream::WriteFrame(AVFrame* frame, AVRational* timeBase)
	{
		frameSink->WriteFrame(streamIndex, frame, timeBase);
	}

	void FrameSinkStream::Close()
	{
		frameSink->Close(streamIndex);
	}

	bool FrameSinkStream::IsPrimed()
	{
		return frameSink->IsPrimed();
	}
}
