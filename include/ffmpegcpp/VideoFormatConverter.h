#pragma once

#include "FFmpegResource.h"

struct AVCodecContext;
struct AVFrame;

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
