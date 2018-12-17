#pragma once

#include "ffmpeg.h"

#include "Sources/InputSource.h"
#include "Sources/Demuxer.h"


namespace ffmpegcpp
{
	class RawAudioFileSource : public InputSource
	{
	public:

		RawAudioFileSource(const char* fileName, const char* inputFormat, int sampleRate, int channels, AudioFrameSink* frameSink);
		virtual ~RawAudioFileSource();

		void PreparePipeline() override;
		bool IsDone() override;
		void Step() override;

	private:

		void CleanUp();

		Demuxer* demuxer = nullptr;
	};


}
