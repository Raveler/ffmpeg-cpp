#pragma once

#include "ffmpeg.h"

#include "Sources/InputSource.h"
#include "Sources/Demuxer.h"

#include <memory>

namespace ffmpegcpp
{
	class RawVideoFileSource : public InputSource
	{
	public:

		RawVideoFileSource(const char* fileName, VideoFrameSink* frameSink);

		// I couldn't get this to work. The thing is that it also crashes weirdly when I run ffmpeg directly,
		// so I think it's more an issue of ffmpeg than one of my library.
		//RawVideoFileSource(const char* fileName, int width, int height, const char* frameRate, AVPixelFormat format, VideoFrameSink* frameSink);

		void PreparePipeline() override;
		bool IsDone() override;
		void Step() override;

	private:

		std::unique_ptr<Demuxer> demuxer;
	};


}
