#pragma once

#include "ffmpeg.h"
#include "FrameSinks/AudioFrameSink.h"

namespace ffmpegcpp
{
	// RawVideoDataSource is used to feed raw memory to the system and process it.
	// You can use this if the video data comes from another source than the file system (ie rendering).
	class RawAudioDataSource
	{

	public:

		RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, int channels, AudioFrameSink* output);
		RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, int channels, int64_t channelLayout, AudioFrameSink* output);
		virtual ~RawAudioDataSource();

		void WriteData(void* data, int sampleCount);
		void Close();

		bool IsPrimed();

	private:

		void CleanUp();

		AudioFrameSink* output;

		AVFrame* frame = nullptr;
	};
}
