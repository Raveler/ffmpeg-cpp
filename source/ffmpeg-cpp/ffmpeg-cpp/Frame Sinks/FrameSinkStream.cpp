#include "FrameSinkStream.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{

	FrameSinkStream::FrameSinkStream(FrameWriter* frameSink, int streamIndex)
	{
		this->frameSink = frameSink;
		this->streamIndex = streamIndex;
	}

	void FrameSinkStream::WriteFrame(AVFrame* frame, StreamData* metaData)
	{
		frameSink->WriteFrame(streamIndex, frame, metaData);
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
