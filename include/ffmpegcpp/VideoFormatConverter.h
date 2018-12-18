#pragma once

#include "ffmpeg.h"
#include "ConvertedAudioProcessor.h"
#include "FFmpegResource.h"

namespace ffmpegcpp
{
	class VideoFormatConverter
	{
	public:

		VideoFormatConverter(AVCodecContext* codecContext);

		AVFrame* ConvertFrame(AVFrame* frame);

	private:

		void InitDelayed(AVFrame* frame);

		AVCodecContext* codecContext;

		bool initialized = false;

		FFmpegResource<AVFrame> converted_frame;
		FFmpegResource<struct SwsContext> swsContext;
	};

}
