#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "Demuxing/AudioInputStream.h"
#include "Demuxing/VideoInputStream.h"
#include "Demuxing/InputStream.h"
#include "Sources/InputSource.h"
#include "FFmpegResource.h"

namespace ffmpegcpp
{
	struct StreamInfo
	{
		int streamId;
		AVCodec* codec;
		AVStream* stream;
	};

	class Demuxer : public InputSource
	{
	public:

		Demuxer(const char* fileName);
		Demuxer(const char* fileName, AVInputFormat* inputFormat, AVDictionary *inputFormatOptions);
		~Demuxer();

		void DecodeBestAudioStream(AudioFrameSink* frameSink);
		void DecodeBestVideoStream(VideoFrameSink* frameSink);

		void DecodeAudioStream(int streamId, AudioFrameSink* frameSink);
		void DecodeVideoStream(int streamId, VideoFrameSink* frameSink);

		std::vector<StreamInfo> GetAudioStreamInfo();
		std::vector<StreamInfo> GetVideoStreamInfo();

		void PreparePipeline() override;
		bool IsDone() override;
		void Step() override;

	private:

		bool done = false;

		const char* fileName;

		std::vector<StreamInfo> GetStreamInfo(AVMediaType mediaType);
		StreamInfo CreateInfo(int streamIndex, AVStream* stream, AVCodec* codec);

		InputStream** inputStreams = nullptr;

		FFmpegResource<AVFormatContext> containerContext;
		FFmpegResource<AVPacket> pkt;

		void DecodePacket();

		void CleanUp();
	};
}
