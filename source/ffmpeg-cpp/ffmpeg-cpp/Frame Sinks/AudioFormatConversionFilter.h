#pragma once

#include "ffmpeg.h"

#include "AudioFrameSink.h"

namespace ffmpegcpp
{
	class AudioFormatConversionFilter : public AudioFrameSink
	{

	public:

		AudioFormatConversionFilter(AudioFrameSink* target);
		~AudioFormatConversionFilter();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);

		virtual AVSampleFormat GetRequiredSampleFormat();

	private:

		void InitDelayed(AVFrame* frame, AVRational* timeBase);

		AudioFrameSink* target;

		AVFrame* filt_frame;

		bool initialized = false;
	};


}
