#pragma once

#include "ffmpeg.h"
#include "ConvertedAudioProcessor.h"

namespace ffmpegcpp
{
	class AudioFormatConverter
	{
	public:

		AudioFormatConverter(ConvertedAudioProcessor* output, AVCodecContext* codecContext);
		~AudioFormatConverter();

		void ProcessFrame(AVFrame* frame);



	private:

		void CleanUp();

		void InitDelayed(AVFrame* frame);

		void WriteCompleteConvertedFrame();

		ConvertedAudioProcessor* output;

		AVCodecContext* codecContext;
		
		bool initialized = false;

		AVFrame* converted_frame = nullptr;
		struct SwrContext* swr_ctx = nullptr;

		int in_sample_rate, out_sample_rate;

		int samples_count = 0;

		int samplesInCurrentFrame = 0;
	};


}
