#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "Sources/AudioInputStream.h"
#include "Sources/VideoInputStream.h"
#include "Sources/InputStream.h"

namespace ffmpegcpp
{
	struct StreamInfo
	{
		int streamId;
		AVCodec* codec;
		AVStream* stream;
	};

	class Demuxer
	{
	public:

		Demuxer(const char* fileName);
		~Demuxer();

		AudioInputStream* GetBestAudioStream(AudioFrameSink* frameSink);
		VideoInputStream* GetBestVideoStream(VideoFrameSink* frameSink);

		AudioInputStream* GetAudioStream(int streamId, AudioFrameSink* frameSink);
		VideoInputStream* GetVideoStream(int streamId, VideoFrameSink* frameSink);

		std::vector<StreamInfo> GetAudioStreamInfo();
		std::vector<StreamInfo> GetVideoStreamInfo();

		void Start();

	private:

		const char* fileName;

		std::vector<StreamInfo> GetStreamInfo(AVMediaType mediaType);
		StreamInfo CreateInfo(int streamIndex, AVStream* stream, AVCodec* codec);

		InputStream** inputStreams;

		AVFormatContext* containerContext = nullptr;
		AVPacket* pkt = nullptr;

		void DecodePacket();

		void CleanUp();
	};
}
