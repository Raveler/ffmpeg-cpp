#pragma once

#include "ffmpeg-cpp/ffmpeg.h"

#include "ffmpeg-cpp/Frame Sinks/VideoFrameSink.h"
#include "ffmpeg-cpp/Codecs/VideoCodec.h"
#include "ffmpeg-cpp/VideoFormatConverter.h"
#include "ffmpeg-cpp/Muxing/Muxer.h"
#include "OneInputFrameSink.h"

namespace ffmpegcpp
{
	class VideoEncoder : public VideoFrameSink, public FrameWriter
	{
	public:
		VideoEncoder(VideoCodec* codec, Muxer* muxer);
		VideoEncoder(VideoCodec* codec, Muxer* muxer, AVPixelFormat format);
		VideoEncoder(VideoCodec* codec, Muxer* muxer, AVRational frameRate);
		VideoEncoder(VideoCodec* codec, Muxer* muxer, AVRational frameRate, AVPixelFormat format);
		virtual ~VideoEncoder();

		FrameSinkStream* CreateStream();

		void WriteFrame(int streamIndex, AVFrame* frame, StreamData* metaData);
		void Close(int streamIndex);

		bool IsPrimed();

	private:

		void OpenLazily(AVFrame* frame, StreamData* metaData);
		void PollCodecForPackets();

		VideoCodec* closedCodec;
		OutputStream* output;

		VideoFormatConverter* formatConverter = nullptr;
		OpenCodec* codec = nullptr;
		AVPacket* pkt = nullptr;

		OneInputFrameSink* oneInputFrameSink = nullptr;

		int frameNumber = 0;

		void CleanUp();

		AVPixelFormat finalPixelFormat = AV_PIX_FMT_NONE;

		AVRational finalFrameRate;
		bool finalFrameRateSet = false;
	};
}

