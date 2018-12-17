#pragma once

#include "ffmpegcpp.h"

using namespace ffmpegcpp;

class GeneratedAudioSource : public InputSource
{
public:

	GeneratedAudioSource(AudioFrameSink* frameSink);
	~GeneratedAudioSource() override;

	void PreparePipeline() override;
	bool IsDone() override;
	void Step() override;

private:

	int sampleRate;
	int channels;
	AVSampleFormat format;

	RawAudioDataSource* output;

	int sampleCount = 735;

	uint16_t* samples;

	int frameNumber = 0;
};

