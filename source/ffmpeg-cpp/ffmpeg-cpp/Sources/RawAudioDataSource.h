#pragma once

#include "ffmpeg.h"
#include "OutputStream.h"
#include "Frame Sinks/VideoFrameSink.h"

namespace ffmpegcpp
{
	// RawVideoDataSource is used to feed raw memory to the system and process it.
	// You can use this if the video data comes from another source than the file system (ie rendering).
	class RawAudioDataSource
	{

	public:

		RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, VideoFrameSink* output);
		~RawAudioDataSource();

		void WriteData(void* data, int sampleCount);

	private:

		void CleanUp();

		VideoFrameSink* output;

		AVFrame* frame = nullptr;
	};
}
