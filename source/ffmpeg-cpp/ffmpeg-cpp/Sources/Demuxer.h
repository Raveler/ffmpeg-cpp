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

		virtual void PreparePipeline();
		virtual bool IsDone();
		virtual void Step();

		ContainerInfo GetInfo();
		int GetFrameCount(int streamId);

	private:

		bool done = false;

		const char* fileName;

		InputStream* GetInputStream(int streamId);

		//std::vector<StreamInfo> GetStreamInfo(AVMediaType mediaType);
		//StreamInfo CreateInfo(int streamIndex, AVStream* stream, AVCodec* codec);

		InputStream** inputStreams = nullptr;

		AVFormatContext* containerContext = nullptr;
		AVPacket* pkt = nullptr;

		void DecodePacket();

		void CleanUp();
	};
}
