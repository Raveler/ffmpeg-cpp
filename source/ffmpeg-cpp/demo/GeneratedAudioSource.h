#pragma once

#include "ffmpegcpp.h"

using namespace ffmpegcpp;

class GeneratedAudioSource : public InputSource
{
public:

	GeneratedAudioSource(AudioFrameSink* frameSink);
	~GeneratedAudioSource();

	void Start();

private:

	int sampleRate;
	int channels;
	AVSampleFormat format;

	RawAudioDataSource* output;
};

