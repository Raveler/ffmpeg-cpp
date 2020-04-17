#pragma once

#include "ffmpeg-cpp/ffmpeg.h"

namespace ffmpegcpp
{
	struct AudioStreamInfo
	{
		int id;
		AVRational timeBase;
		const AVCodec* codec;
		float bitRate;

		int sampleRate;
		int channels;

		uint64_t channelLayout;
		char channelLayoutName[255];

	};
}
