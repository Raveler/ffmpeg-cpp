#pragma once

#include "ffmpeg.h"

#include "Frame Sinks/AudioFrameSink.h"
#include "Codecs/Codec.h"
#include "AudioOutputStream.h"

namespace ffmpegcpp
{
	class AudioEncoder : public AudioFrameSink
	{
	public:
		AudioEncoder(AudioOutputStream* output);
		~AudioEncoder();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

		AVSampleFormat GetRequiredSampleFormat();

	private:

		void InitDelayed(AVFrame* frame, AVRational* timeBase);

		AVFrame* ConvertToDestinationFormat(AVFrame* frame, AVRational* timeBase);

		OpenCodec* codec;
		AudioOutputStream* output;

		AVFrame* converted_frame;

		AVPacket* pkt;

		struct SwrContext *swr_ctx;

		int frameNumber = 0;

		int samples_count = 0;

		bool initialized = false;

		int in_sample_rate, out_sample_rate;
	};
}

