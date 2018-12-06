#pragma once

#include "ffmpeg.h"
#include "OutputStream.h"
#include "Frame Sinks/AudioFrameSink.h"

namespace ffmpegcpp
{
	// RawVideoDataSource is used to feed raw memory to the system and process it.
	// You can use this if the video data comes from another source than the file system (ie rendering).
	class RawAudioDataSource
	{

	public:

		RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, int channels, AudioFrameSink* output);
		RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, int channels, int64_t channelLayout, AudioFrameSink* output);
		~RawAudioDataSource();

		void WriteData(void* data, int sampleCount);
		void Close();

	private:

		void CleanUp();

		AudioFrameSink* output;

		AVAudioFifo* fifo = nullptr;
		AVFrame* frame = nullptr;
	};
}
