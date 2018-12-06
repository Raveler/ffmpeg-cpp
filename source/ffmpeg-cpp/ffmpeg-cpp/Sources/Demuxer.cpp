#include "Demuxer.h"
#include "FFmpegException.h"
#include "CodecDeducer.h"

#include <string>

using namespace std;

namespace ffmpegcpp
{

	Demuxer::Demuxer(const char* fileName)
		: Demuxer(fileName, NULL, NULL)
	{
	}

	Demuxer::Demuxer(const char* fileName, AVInputFormat* inputFormat, AVDictionary *format_opts)
	{
		this->fileName = fileName;

		// open input file, and allocate format context
		int ret;
		if ((ret = avformat_open_input(&containerContext, fileName, inputFormat, &format_opts)) < 0)
		{
			CleanUp();
			throw FFmpegException("Failed to open input container" + string(fileName), ret);
		}

		// retrieve stream information
		if (ret = (avformat_find_stream_info(containerContext, NULL)) < 0)
		{
			CleanUp();
			throw FFmpegException("Failed to read streams from " + string(fileName), ret);
		}

		inputStreams = new InputStream*[containerContext->nb_streams];
		for (int i = 0; i < containerContext->nb_streams; ++i)
		{
			inputStreams[i] = nullptr;
		}

		// initialize packet, set data to NULL, let the demuxer fill it
		pkt = av_packet_alloc();
		if (!pkt)
		{
			CleanUp();
			throw FFmpegException("Failed to create packet for input stream");
		}
		av_init_packet(pkt);
		pkt->data = NULL;
		pkt->size = 0;
	}

	Demuxer::~Demuxer()
	{
		CleanUp();
	}

	void Demuxer::CleanUp()
	{
		if (inputStreams != nullptr)
		{
			for (int i = 0; i < containerContext->nb_streams; ++i)
			{
				if (inputStreams[i] != nullptr)
				{
					delete inputStreams[i];
				}
			}
			inputStreams = nullptr;
		}
		if (containerContext != nullptr)
		{
			avformat_close_input(&containerContext);
			containerContext = nullptr;
		}
		if (pkt != nullptr)
		{
			av_packet_free(&pkt);
			pkt = nullptr;
		}
	}

	vector<StreamInfo> Demuxer::GetAudioStreamInfo()
	{
		return GetStreamInfo(AVMEDIA_TYPE_AUDIO);
	}

	vector<StreamInfo> Demuxer::GetVideoStreamInfo()
	{
		return GetStreamInfo(AVMEDIA_TYPE_VIDEO);
	}

	vector<StreamInfo> Demuxer::GetStreamInfo(AVMediaType mediaType)
	{
		vector<StreamInfo> streamInfo;
		for (int i = 0; i < containerContext->nb_streams; ++i)
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

	StreamInfo Demuxer::CreateInfo(int streamIndex, AVStream* stream, AVCodec* codec)
	{
		StreamInfo info;
		info.streamId = streamIndex;
		info.stream = stream;
		info.codec = codec;
		return info;
	}

	void Demuxer::EncodeBestAudioStream(AudioFrameSink* frameSink)
	{
		int ret = av_find_best_stream(containerContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
		if (ret < 0)
		{
			throw FFmpegException("Could not find " + string(av_get_media_type_string(AVMEDIA_TYPE_AUDIO)) + " stream in input file " + fileName, ret);
		}
		int streamIndex = ret;
		return EncodeAudioStream(streamIndex, frameSink);
	}

	void Demuxer::EncodeBestVideoStream(VideoFrameSink* frameSink)
	{
		int ret = av_find_best_stream(containerContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		if (ret < 0)
		{
			throw FFmpegException("Could not find " + string(av_get_media_type_string(AVMEDIA_TYPE_VIDEO)) + " stream in input file " + fileName, ret);
		}
		int streamIndex = ret;
		return EncodeVideoStream(streamIndex, frameSink);
	}

	void Demuxer::EncodeAudioStream(int streamIndex, AudioFrameSink* frameSink)
	{
		// each input stream can only be used once
		if (inputStreams[streamIndex] != nullptr)
		{
			throw FFmpegException("That stream is already tied to a frame sink, you cannot process the same stream multiple times");
		}

		// create the stream
		AVStream* stream = containerContext->streams[streamIndex];
		AudioInputStream* inputStream = new AudioInputStream(frameSink, stream);
		inputStream->Open();

		// remember and return
		inputStreams[streamIndex] = inputStream;
	}

	void Demuxer::EncodeVideoStream(int streamIndex, VideoFrameSink* frameSink)
	{
		// each input stream can only be used once
		if (inputStreams[streamIndex] != nullptr)
		{
			throw FFmpegException("That stream is already tied to a frame sink, you cannot process the same stream multiple times");
		}

		// create the stream
		AVStream* stream = containerContext->streams[streamIndex];
		VideoInputStream* inputStream = new VideoInputStream(frameSink, stream);
		inputStream->Open();

		// remember and return
		inputStreams[streamIndex] = inputStream;
	}

	void Demuxer::Start()
	{
		// read frames from the file
		int ret = 0;
		while (ret >= 0)
		{
			ret = av_read_frame(containerContext, pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				continue;
			else if (ret < 0)
			{
				throw FFmpegException("Error during demuxing", ret);
			}

			DecodePacket();
		}

		// flush cached frames
		pkt->data = NULL;
		pkt->size = 0;
		for (int i = 0; i < containerContext->nb_streams; ++i)
		{
			InputStream* stream = inputStreams[i];
			if (stream != nullptr)
			{
				pkt->stream_index = i;
				DecodePacket();
			}
		}
	}

	void Demuxer::DecodePacket()
	{
		int streamIndex = pkt->stream_index;
		InputStream* inputStream = inputStreams[streamIndex];

		if (inputStream != nullptr)
		{
			inputStream->DecodePacket(pkt);
		}
	}
}