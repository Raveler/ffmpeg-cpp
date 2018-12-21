#pragma once

#include "ffmpegcpp.h"

using namespace ffmpegcpp;

class GeneratedVideoSource : public InputSource
{
public:

	GeneratedVideoSource(int width, int height, VideoFrameSink* frameSink);
	~GeneratedVideoSource();

	virtual void PreparePipeline();
	virtual bool IsDone();
	virtual void Step();

private:

	RawVideoDataSource* output;

	int frameNumber = 0;

	uint8_t *rgb = NULL;
};

