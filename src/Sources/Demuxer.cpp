#include "Sources/Demuxer.h"

#include "CodecDeducer.h"
#include "Demuxing/AudioInputStream.h"
#include "Demuxing/InputStream.h"
#include "Demuxing/VideoInputStream.h"
#include "ffmpeg.h"
#include "FFmpegException.h"

#include <algorithm>
#include <string>

using namespace std;

namespace ffmpegcpp
{

	Demuxer::Demuxer(const char* fileName)
		: Demuxer(fileName, nullptr, nullptr)
	{
	}

	Demuxer::Demuxer(const char* fileName, AVInputFormat* inputFormat, AVDictionary *format_opts)
	{
		this->fileName = fileName;

		// open input file, and allocate format context
		int ret;
        auto container_ctx = containerContext.get();
		if ((ret = avformat_open_input(&container_ctx, fileName, inputFormat, &format_opts)) < 0)
		{
			throw FFmpegException("Failed to open input container " + string(fileName), ret);
		}

		// retrieve stream information
		if (ret = (avformat_find_stream_info(containerContext.get(), nullptr)) < 0)
		{
			throw FFmpegException("Failed to read streams from " + string(fileName), ret);
		}

        for (int i = 0; i < containerContext->nb_streams; ++i)
        {
            inputStreams.emplace_back(nullptr);
        }

		// initialize packet, set data to NULL, let the demuxer fill it
		pkt = MakeFFmpegResource<AVPacket>(av_packet_alloc());
		if (!pkt)
		{
			throw FFmpegException("Failed to create packet for input stream");
		}
		av_init_packet(pkt.get());
		pkt->data = nullptr;
		pkt->size = 0;
	}

	vector<StreamInfo> Demuxer::GetAudioStreamInfo() const
	{
		return GetStreamInfo(AVMEDIA_TYPE_AUDIO);
	}

	vector<StreamInfo> Demuxer::GetVideoStreamInfo() const
	{
		return GetStreamInfo(AVMEDIA_TYPE_VIDEO);
	}

	vector<StreamInfo> Demuxer::GetStreamInfo(AVMediaType mediaType) const
	{
		vector<StreamInfo> streamInfo;
		for (unsigned int i = 0; i < containerContext->nb_streams; ++i)
		{
			AVStream* stream = containerContext->streams[i];

			/* find decoder for the stream */
			AVCodec* codec = CodecDeducer::DeduceDecoder(stream->codecpar->codec_id);
			if (!codec)
			{
				throw FFmpegException(string("Failed to deduce codec for stream ") + std::to_string(i) + " in container");
			}

			if (codec->type == mediaType)
			{
				streamInfo.push_back(CreateInfo(i, stream, codec));
			}
		}
		return streamInfo;
	}

	StreamInfo Demuxer::CreateInfo(int streamIndex, AVStream* stream, AVCodec* codec) const
	{
		StreamInfo info;
		info.streamId = streamIndex;
		info.stream = stream;
		info.codec = codec;
		return info;
	}

	void Demuxer::DecodeBestAudioStream(AudioFrameSink* frameSink)
	{
		int ret = av_find_best_stream(containerContext.get(), AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
		if (ret < 0)
		{
			throw FFmpegException("Could not find " + string(av_get_media_type_string(AVMEDIA_TYPE_AUDIO)) + " stream in input file " + fileName, ret);
		}
		int streamIndex = ret;
		return DecodeAudioStream(streamIndex, frameSink);
	}

	void Demuxer::DecodeBestVideoStream(VideoFrameSink* frameSink)
	{
		int ret = av_find_best_stream(containerContext.get(), AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
		if (ret < 0)
		{
			throw FFmpegException("Could not find " + string(av_get_media_type_string(AVMEDIA_TYPE_VIDEO)) + " stream in input file " + fileName, ret);
		}
		int streamIndex = ret;
		return DecodeVideoStream(streamIndex, frameSink);
	}

	void Demuxer::DecodeAudioStream(int streamIndex, AudioFrameSink* frameSink)
	{
		// each input stream can only be used once
		if (inputStreams[streamIndex] != nullptr)
		{
			throw FFmpegException("That stream is already tied to a frame sink, you cannot process the same stream multiple times");
		}

		// create the stream
		AVStream* stream = containerContext->streams[streamIndex];
		auto inputStream = std::make_unique<AudioInputStream>(frameSink, stream);
		inputStream->Open();

		// remember and return
		inputStreams[streamIndex] = std::move(inputStream);
	}

	void Demuxer::DecodeVideoStream(int streamIndex, VideoFrameSink* frameSink)
	{
		// each input stream can only be used once
		if (inputStreams[streamIndex] != nullptr)
		{
			throw FFmpegException("That stream is already tied to a frame sink, you cannot process the same stream multiple times");
		}

		// create the stream
		AVStream* stream = containerContext->streams[streamIndex];
		auto inputStream = std::make_unique<VideoInputStream>(frameSink, stream);
		inputStream->Open();

		// remember and return
		inputStreams[streamIndex] = std::move(inputStream);
	}

	void Demuxer::PreparePipeline()
	{
		bool allPrimed = false;
		do
		{
			Step();

			// see if all input streams are primed
			allPrimed = std::all_of(cbegin(inputStreams), cend(inputStreams), [](const auto & stream){
				return stream->IsPrimed();
			});

		} while (!allPrimed && !IsDone());
	}

	bool Demuxer::IsDone() const
	{
		return done;
	}

	void Demuxer::Step()
	{
		// read frames from the file
		int ret = av_read_frame(containerContext.get(), pkt.get());

		// EOF
		if (ret == AVERROR_EOF)
		{
			pkt->data = nullptr;
			pkt->size = 0;
			for (unsigned int i = 0; i < containerContext->nb_streams; ++i)
			{
				auto & stream = inputStreams[i];
				if (stream != nullptr)
				{
					pkt->stream_index = i;
					DecodePacket();
					stream->Close();
				}
			}

			done = true;
			return;
		}

		// not ready yet
		if (ret == AVERROR(EAGAIN)) return;

		// error
		if (ret < 0)
		{
			throw FFmpegException("Error during demuxing", ret);
		}

		// decode the finished packet
		DecodePacket();
	}

	void Demuxer::DecodePacket()
	{
		int streamIndex = pkt->stream_index;
		const auto & inputStream = inputStreams[streamIndex];

		if (inputStream != nullptr)
		{
			inputStream->DecodePacket(pkt.get());
		}

		// We need to unref the packet here because packets might pass by here
		// that don't have a stream attached to them. We want to dismiss them!
		av_packet_unref(pkt.get());
	}
}