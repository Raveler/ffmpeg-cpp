#pragma once

#include "ffmpeg.h"
#include "ConvertedAudioProcessor.h"
#include "FFmpegResource.h"

namespace ffmpegcpp
{
	class AudioFormatConverter
	{
	public:

		AudioFormatConverter(ConvertedAudioProcessor* output, AVCodecContext* codecContext);

		void ProcessFrame(AVFrame* frame);

	private:

		void InitDelayed(AVFrame* frame);

		void AddToFifo(AVFrame* frame);
		void PullConvertedFrameFromFifo();

		void WriteCompleteConvertedFrame();

		ConvertedAudioProcessor* output;

		AVCodecContext* codecContext;
		
		bool initialized = false;

		FFmpegResource<AVAudioFifo> fifo;
		FFmpegResource<AVFrame> tmp_frame;
		FFmpegResource<AVFrame> converted_frame;
		FFmpegResource<struct SwrContext> swr_ctx;

		int in_sample_rate, out_sample_rate;

		int samples_count = 0;

		int samplesInCurrentFrame = 0;
	};


}
