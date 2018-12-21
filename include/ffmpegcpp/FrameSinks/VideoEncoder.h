#pragma once

#include "FFmpegResource.h"
#include "FrameSinks/VideoFrameSink.h"

#include <memory>

enum AVPixelFormat;
struct AVRational;
struct AVFrame;
struct AVPacket;

namespace ffmpegcpp
{
	class VideoCodec;
	class Muxer;
	class OutputStream;
	class VideoFormatConverter;
	class OpenCodec;


	class VideoEncoder : public VideoFrameSink
	{
	public:
		VideoEncoder(VideoCodec* codec, Muxer* muxer);
		VideoEncoder(VideoCodec* codec, Muxer* muxer, AVPixelFormat format);
		VideoEncoder(VideoCodec* codec, Muxer* muxer, AVRational frameRate);
		VideoEncoder(VideoCodec* codec, Muxer* muxer, AVRational frameRate, AVPixelFormat format);

		void WriteFrame(AVFrame* frame, AVRational* timeBase) override;
		void Close() override;

		bool IsPrimed() const override;

	private:

		void OpenLazily(AVFrame* frame, AVRational* timeBase);
		void PollCodecForPackets();

		VideoCodec* closedCodec;
		std::unique_ptr<OutputStream> output;

		std::unique_ptr<VideoFormatConverter> formatConverter;
		std::unique_ptr<OpenCodec> codec;
		FFmpegResource<AVPacket> pkt;

		int frameNumber = 0;

		AVPixelFormat finalPixelFormat = AV_PIX_FMT_NONE;

		AVRational finalFrameRate;
		bool finalFrameRateSet = false;
	};
}

