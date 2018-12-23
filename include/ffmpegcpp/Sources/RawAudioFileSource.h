#pragma once

#include "Sources/InputSource.h"

#include <memory>
#include <string>

namespace ffmpegcpp
{
	class AudioFrameSink;
	class Demuxer;

	class RawAudioFileSource : public InputSource
	{
	public:

		RawAudioFileSource(const std::string & fileName, const std::string & inputFormat, int sampleRate, int channels, AudioFrameSink* frameSink);

		void PreparePipeline() override;
		bool IsDone() const override;
		void Step() override;

	private:

		std::unique_ptr<Demuxer> demuxer;
	};


}
