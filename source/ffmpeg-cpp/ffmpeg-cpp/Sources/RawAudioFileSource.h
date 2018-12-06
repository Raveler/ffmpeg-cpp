#pragma once

#include "ffmpeg.h"

#include "InputSource.h"
#include "Demuxer.h"


namespace ffmpegcpp
{
	class RawAudioFileSource : public InputSource
	{
	public:

		RawAudioFileSource(const char* fileName, const char* inputFormat, int sampleRate, int channels, AudioFrameSink* frameSink);
		~RawAudioFileSource();

		void Start();

	private:

		void CleanUp();

		Demuxer* demuxer = nullptr;
	};


}
