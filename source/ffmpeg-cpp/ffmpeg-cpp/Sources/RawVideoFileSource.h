#pragma once

#include "ffmpeg.h"

#include "InputSource.h"
#include "Demuxer.h"


namespace ffmpegcpp
{
	class RawVideoFileSource : public InputSource
	{
	public:

		RawVideoFileSource(const char* fileName, VideoFrameSink* frameSink);
		virtual ~RawVideoFileSource();

		virtual void PreparePipeline();
		virtual bool IsDone();
		virtual void Step();

	private:

		void CleanUp();

		Demuxer* demuxer = nullptr;
	};


}
