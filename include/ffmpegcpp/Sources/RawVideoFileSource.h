#pragma once

#include "Sources/InputSource.h"

#include <memory>
#include <string>

namespace ffmpegcpp
{
	class Demuxer;
	class VideoFrameSink;

	class RawVideoFileSource : public InputSource
	{
	public:

		RawVideoFileSource(const std::string & fileName, VideoFrameSink* frameSink);

		// I couldn't get this to work. The thing is that it also crashes weirdly when I run ffmpeg directly,
		// so I think it's more an issue of ffmpeg than one of my library.
		//RawVideoFileSource(const char* fileName, int width, int height, const char* frameRate, AVPixelFormat format, VideoFrameSink* frameSink);

		void PreparePipeline() override;
		bool IsDone() const override;
		void Step() override;

	private:

		std::unique_ptr<Demuxer> demuxer;
	};


}
