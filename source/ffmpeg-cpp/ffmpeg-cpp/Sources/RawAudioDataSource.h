#pragma once

#include "ffmpeg-cpp/ffmpeg.h"
#include "ffmpeg-cpp/Frame Sinks/AudioFrameSink.h"

namespace ffmpegcpp
{
	// RawVideoDataSource is used to feed raw memory to the system and process it.
	// You can use this if the video data comes from another source than the file system (ie rendering).
	class RawAudioDataSource
	{

	public:

		RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, int channels, FrameSink* output);
		RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, int channels, int64_t channelLayout, FrameSink* output);
		virtual ~RawAudioDataSource();

		void WriteData(void* data, int sampleCount);
		void Close();

		bool IsPrimed();

	private:

		void CleanUp();

		FrameSinkStream* output;

		AVFrame* frame = nullptr;

		StreamData* metaData = nullptr;
	};
}
