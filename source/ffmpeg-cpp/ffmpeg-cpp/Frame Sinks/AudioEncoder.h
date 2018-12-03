#pragma once

#include "ffmpeg.h"
#include "std.h"

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

		void CleanUp();

		void InitDelayed(AVFrame* frame, AVRational* timeBase);

		void ConvertToDestinationFormat(AVFrame* frame, AVRational* timeBase);

		void CreateConvertedFrame(int nb_samples);

		void WriteConvertedFrame(AVFrame* frame);

		OpenCodec* codec;
		AudioOutputStream* output;

		AVFrame* converted_frame = nullptr;
		AVPacket* pkt = nullptr;
		struct SwrContext* swr_ctx = nullptr;

		int frameNumber = 0;

		int samples_count = 0;

		bool initialized = false;

		int in_sample_rate, out_sample_rate;
	};
}

