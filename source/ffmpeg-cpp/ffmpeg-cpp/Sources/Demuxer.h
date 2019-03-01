#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "Demuxing/AudioInputStream.h"
#include "Demuxing/VideoInputStream.h"
#include "Demuxing/InputStream.h"
#include "Sources/InputSource.h"
#include "Info/ContainerInfo.h"

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

		virtual void PreparePipeline();
		virtual bool IsDone();
		virtual void Step();

		ContainerInfo GetInfo();

	private:

		bool done = false;

		const char* fileName;

		std::vector<StreamInfo> GetStreamInfo(AVMediaType mediaType);
		StreamInfo CreateInfo(int streamIndex, AVStream* stream, AVCodec* codec);

		InputStream** inputStreams = nullptr;

		AVFormatContext* containerContext = nullptr;
		AVPacket* pkt = nullptr;

		void DecodePacket();

		void CleanUp();
	};
}
