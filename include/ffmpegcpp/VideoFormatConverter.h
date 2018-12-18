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
		~VideoFormatConverter();

		AVFrame* ConvertFrame(AVFrame* frame);



	private:

		void CleanUp();

		void InitDelayed(AVFrame* frame);


		AVCodecContext* codecContext;

		bool initialized = false;

		FFmpegResource<AVFrame> converted_frame;
		struct SwsContext* swsContext = nullptr;
	};


}
