#pragma once

extern "C" {
	struct AVFormatContext;
	struct AVStream;
	struct AVPacket;
	struct AVFrame;
	struct AVCodecContext;
	struct AVAudioFifo;
	struct AVCodec;
	struct AVInputFormat;
	struct AVStream;
	struct AVOutputFormat;
	struct AVFilterGraph;
	struct AVFilterContext;
	struct AVFifoBuffer;
	//struct SwsContext;

	#include <libavutil/rational.h>
	#include <libavutil/error.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswresample/swresample.h>
	#include <libswscale/swscale.h>
}