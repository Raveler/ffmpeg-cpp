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
		~RawVideoFileSource();

		void Start();

	private:

		void CleanUp();

		Demuxer* demuxer = nullptr;
	};


}
