#pragma once

#include "Demuxing/InputStream.h"
#include "Sources/InputSource.h"
#include "FFmpegResource.h"

#include <memory>
#include <vector>

enum AVMediaType;
struct AVCodec;
struct AVDictionary;
struct AVFormatContext;
struct AVInputFormat;
struct AVPacket;
struct AVStream;

namespace ffmpegcpp
{
	class AudioFrameSink;
	class VideoFrameSink;

	struct StreamInfo
	{
        unsigned int streamId;
		AVCodec* codec;
		AVStream* stream;
	};

	class Demuxer : public InputSource
	{
	public:

		Demuxer(const std::string & fileName, AVInputFormat* inputFormat = nullptr, AVDictionary *inputFormatOptions = nullptr);

		void DecodeBestAudioStream(AudioFrameSink* frameSink);
		void DecodeBestVideoStream(VideoFrameSink* frameSink);

		void DecodeAudioStream(int streamId, AudioFrameSink* frameSink);
		void DecodeVideoStream(int streamId, VideoFrameSink* frameSink);

		std::vector<StreamInfo> GetAudioStreamInfo() const;
		std::vector<StreamInfo> GetVideoStreamInfo() const;

		void PreparePipeline() override;
		bool IsDone() const override;
		void Step() override;

	private:

		std::vector<StreamInfo> GetStreamInfo(AVMediaType mediaType) const;

		void DecodePacket();

		bool done = false;

		std::string fileName;

		std::vector<std::unique_ptr<InputStream>> inputStreams;

		FFmpegResource<AVFormatContext> containerContext;
		FFmpegResource<AVPacket> pkt;

	};
}
