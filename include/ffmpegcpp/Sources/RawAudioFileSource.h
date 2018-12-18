#pragma once

#include "ffmpeg.h"

#include "Sources/InputSource.h"
#include "Sources/Demuxer.h"

#include <memory>

namespace ffmpegcpp
{
	class RawAudioFileSource : public InputSource
	{
	public:

		RawAudioFileSource(const char* fileName, const char* inputFormat, int sampleRate, int channels, AudioFrameSink* frameSink);

		void PreparePipeline() override;
		bool IsDone() override;
		void Step() override;

	private:

		std::unique_ptr<Demuxer> demuxer;
	};


}
