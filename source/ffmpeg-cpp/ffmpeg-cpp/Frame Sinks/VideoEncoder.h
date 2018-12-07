#pragma once

#include "ffmpeg.h"

#include "Frame Sinks/VideoFrameSink.h"
#include "Codecs/VideoCodec.h"
#include "VideoFormatConverter.h"
#include "Muxing/Muxer.h"

namespace ffmpegcpp
{
	class VideoEncoder : public VideoFrameSink
	{
	public:
		VideoEncoder(VideoCodec* codec, Muxer* muxer);
		VideoEncoder(VideoCodec* codec, Muxer* muxer, AVPixelFormat format);
		VideoEncoder(VideoCodec* codec, Muxer* muxer, AVRational frameRate);
		VideoEncoder(VideoCodec* codec, Muxer* muxer, AVRational frameRate, AVPixelFormat format);
		virtual ~VideoEncoder();

		void WriteFrame(AVFrame* frame, AVRational* timeBase);
		void Close();

	private:

		void OpenLazily(AVFrame* frame, AVRational* timeBase);
		void PollCodecForPackets();

		VideoCodec* closedCodec;
		OutputStream* output;

		VideoFormatConverter* formatConverter = nullptr;
		OpenCodec* codec = nullptr;
		AVPacket* pkt = nullptr;

		int frameNumber = 0;

		void CleanUp();

		AVPixelFormat finalPixelFormat = AV_PIX_FMT_NONE;

		AVRational finalFrameRate;
		bool finalFrameRateSet = false;
	};
}

