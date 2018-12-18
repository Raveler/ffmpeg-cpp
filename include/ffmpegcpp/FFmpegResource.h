#pragma once

#include <memory>

#include "ffmpeg.h"

namespace ffmpegcpp
{

	template <typename value_type> struct Deleter {};

    template <>
	struct Deleter<AVCodecContext> {
		void operator()(AVCodecContext * ptr) { avcodec_free_context(&ptr); }
	};

    template <>
	struct Deleter<AVPacket> {
		void operator()(AVPacket * ptr) { av_packet_free(&ptr); }
	};

    template <>
	struct Deleter<AVFrame> {
		void operator()(AVFrame * ptr) { av_frame_free(&ptr); }
	};


    template <>
	struct Deleter<AVFilterGraph> {
		void operator()(AVFilterGraph * ptr) { avfilter_graph_free(&ptr); }
	};


	template <typename value_type>
	class FFmpegResource : public std::unique_ptr<value_type, Deleter<value_type>>
	{
    public:
        FFmpegResource() : std::unique_ptr<value_type, Deleter<value_type>>() {}
        FFmpegResource(value_type * ptr) : std::unique_ptr<value_type, Deleter<value_type>>(ptr) {}
	};

	template <typename value_type>
	FFmpegResource<value_type> MakeFFmpegResource(value_type * ptr)
	{
		return FFmpegResource<value_type>(ptr);
	}
}
