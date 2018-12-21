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

		virtual void PreparePipeline();
		virtual bool IsDone();
		virtual void Step();

	private:

		void CleanUp();

		Demuxer* demuxer = nullptr;
	};


}
