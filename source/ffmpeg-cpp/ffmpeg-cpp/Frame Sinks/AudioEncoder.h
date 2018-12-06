#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "Frame Sinks/AudioFrameSink.h"
#include "Codecs/Codec.h"
#include "AudioOutputStream.h"
#include "ConvertedAudioProcessor.h"
#include "AudioFormatConverter.h"

namespace ffmpegcpp
{
	class AudioEncoder : public AudioFrameSink, public ConvertedAudioProcessor
	{
	public:
		AudioEncoder(AudioOutputStream* output);
		~AudioEncoder();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

		AVSampleFormat GetRequiredSampleFormat();

		virtual void WriteConvertedFrame(AVFrame* frame);

	private:

		void CleanUp();

		AudioFormatConverter *formatConverter = nullptr;


		OpenCodec* codec;
		AudioOutputStream* output;

		AVPacket* pkt = nullptr;

		int frameNumber = 0;
	};
}

