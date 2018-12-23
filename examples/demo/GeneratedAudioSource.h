#pragma once

#include <memory>

#include "ffmpegcpp.h"

class GeneratedAudioSource : public ffmpegcpp::InputSource
{
public:

	GeneratedAudioSource(ffmpegcpp::AudioFrameSink* frameSink);
	~GeneratedAudioSource();

	void PreparePipeline() override;
	bool IsDone() const override;
	void Step() override;

private:

	int sampleRate;
	int channels;
	AVSampleFormat format;

	std::unique_ptr<ffmpegcpp::RawAudioDataSource> output;

	int sampleCount = 735;

	uint16_t* samples;

	int frameNumber = 0;
};

