#pragma once

#include "ffmpegcpp.h"

using namespace ffmpegcpp;

class GeneratedAudioSource : public InputSource
{
public:

	GeneratedAudioSource(FrameSink* frameSink);
	~GeneratedAudioSource();

	virtual void PreparePipeline();
	virtual bool IsDone();
	virtual void Step();

private:

	int sampleRate;
	int channels;
	AVSampleFormat format;

	RawAudioDataSource* output;

	int sampleCount = 735;

	uint16_t* samples;

	int frameNumber = 0;
};

