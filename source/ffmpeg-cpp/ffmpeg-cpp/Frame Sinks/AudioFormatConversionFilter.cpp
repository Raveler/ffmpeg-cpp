#include "AudioFormatConversionFilter.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{

	AudioFormatConversionFilter::AudioFormatConversionFilter(AudioFrameSink* target)
	{
		this->target = target;
	}

	AudioFormatConversionFilter::~AudioFormatConversionFilter()
	{
		av_frame_free(&filt_frame);
	}

	void AudioFormatConversionFilter::InitDelayed(AVFrame* frame, AVRational* timeBase)
	{
		filt_frame = av_frame_alloc();
		// TODO
	}

	void AudioFormatConversionFilter::WriteFrame(AVFrame* frame, AVRational* timeBase)
	{
		// lazily initialize because we need the data from the frame to configure our filter graph
		if (!initialized)
		{
			InitDelayed(frame, timeBase);
			initialized = true;
		}
		// TODO
	}

	AVSampleFormat AudioFormatConversionFilter::GetRequiredSampleFormat()
	{
		return AV_SAMPLE_FMT_NONE;
	}
}
