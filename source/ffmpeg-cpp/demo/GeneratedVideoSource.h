#pragma once

#include "ffmpegcpp.h"

using namespace ffmpegcpp;

class GeneratedVideoSource : public InputSource
{
public:

	GeneratedVideoSource(int width, int height, VideoFrameSink* frameSink);
	~GeneratedVideoSource();

	void Start();

private:

	RawVideoDataSource* output;
};

