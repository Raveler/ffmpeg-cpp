#pragma once

#include <memory>

#include "ffmpegcpp.h"

class GeneratedVideoSource : public ffmpegcpp::InputSource
{
public:

	GeneratedVideoSource(int width, int height, ffmpegcpp::VideoFrameSink* frameSink);
	~GeneratedVideoSource();

	void PreparePipeline() override;
	bool IsDone() const override;
	void Step() override;

private:

	std::unique_ptr<ffmpegcpp::RawVideoDataSource> output;

	int frameNumber = 0;

	uint8_t *rgb = nullptr;
};

