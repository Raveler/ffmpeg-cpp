#pragma once

#include "ffmpeg-cpp/ffmpeg.h"

#include "InputSource.h"
#include "Demuxer.h"


namespace ffmpegcpp
{
	class RawAudioFileSource : public InputSource
	{
	public:

		RawAudioFileSource(const char* fileName, const char* inputFormat, int sampleRate, int channels, FrameSink* frameSink);
		virtual ~RawAudioFileSource();

		virtual void PreparePipeline();
		virtual bool IsDone();
		virtual void Step();

	private:

		void CleanUp();

		Demuxer* demuxer = nullptr;
	};


}
