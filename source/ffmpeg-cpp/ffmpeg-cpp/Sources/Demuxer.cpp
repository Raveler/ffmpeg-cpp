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
			throw FFmpegException("Failed to open input container " + string(fileName), ret);
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
			delete inputStreams;
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

	void Demuxer::DecodeBestAudioStream(FrameSink* frameSink)
	{
		int ret = av_find_best_stream(containerContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
		if (ret < 0)
		{
			throw FFmpegException("Could not find " + string(av_get_media_type_string(AVMEDIA_TYPE_AUDIO)) + " stream in input file " + fileName, ret);
		}
		int streamIndex = ret;
		return DecodeAudioStream(streamIndex, frameSink);
	}

	void Demuxer::DecodeBestVideoStream(FrameSink* frameSink)
	{
		int ret = av_find_best_stream(containerContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		if (ret < 0)
		{
			throw FFmpegException("Could not find " + string(av_get_media_type_string(AVMEDIA_TYPE_VIDEO)) + " stream in input file " + fileName, ret);
		}
		int streamIndex = ret;
		return DecodeVideoStream(streamIndex, frameSink);
	}

	void Demuxer::DecodeAudioStream(int streamIndex, FrameSink* frameSink)
	{
		// each input stream can only be used once
		if (inputStreams[streamIndex] != nullptr)
		{
			throw FFmpegException("That stream is already tied to a frame sink, you cannot process the same stream multiple times");
		}

		// create the stream
		InputStream* inputStream = GetInputStream(streamIndex);
		inputStream->Open(frameSink);

		// remember and return
		inputStreams[streamIndex] = inputStream;
	}

	void Demuxer::DecodeVideoStream(int streamIndex, FrameSink* frameSink)
	{
		// each input stream can only be used once
		if (inputStreams[streamIndex] != nullptr)
		{
			throw FFmpegException("That stream is already tied to a frame sink, you cannot process the same stream multiple times");
		}

		// create the stream
		InputStream* inputStream = GetInputStream(streamIndex);
		inputStream->Open(frameSink);

		// remember and return
		inputStreams[streamIndex] = inputStream;
	}

	InputStream* Demuxer::GetInputStream(int streamIndex)
	{
		// already exists
		if (inputStreams[streamIndex] != nullptr) return inputStreams[streamIndex];

		// The stream doesn't exist but we already processed all our frames, so it makes no sense
		// to add it anymore.
		if (IsDone()) return nullptr;

		AVStream* stream = containerContext->streams[streamIndex];
		AVCodec* codec = CodecDeducer::DeduceDecoder(stream->codecpar->codec_id);
		if (codec == nullptr) return nullptr; // no codec found - we can't really do anything with this stream!
		switch (codec->type)
		{
		case AVMEDIA_TYPE_VIDEO:
			inputStreams[streamIndex] = new VideoInputStream(containerContext, stream);
			break;
		case AVMEDIA_TYPE_AUDIO:
			inputStreams[streamIndex] = new AudioInputStream(containerContext, stream);
			break;
		}

		// return the created stream
		return inputStreams[streamIndex];
	}

	InputStream* Demuxer::GetInputStreamById(int streamId)
	{
		// map the stream id to an index by going over all the streams and comparing the id
		for (int i = 0; i < containerContext->nb_streams; ++i)
		{
			AVStream* stream = containerContext->streams[i];
			if (stream->id == streamId) return GetInputStream(i);
		}

		// no match found
		return nullptr;
	}

	void Demuxer::PreparePipeline()
	{
		bool allPrimed = false;
		do
		{
			Step();

			// see if all input streams are primed
			allPrimed = true;
			for (int i = 0; i < containerContext->nb_streams; ++i)
			{
				InputStream* stream = inputStreams[i];
				if (stream != nullptr)
				{
					if (!stream->IsPrimed()) allPrimed = false;
				}
			}
			
		} while (!allPrimed && !IsDone());
	}

	bool Demuxer::IsDone()
	{
		return done;
	}

	void Demuxer::Step()
	{
		// read frames from the file
		int ret = av_read_frame(containerContext, pkt);

		// EOF
		if (ret == AVERROR_EOF)
		{
			pkt->data = NULL;
			pkt->size = 0;
			for (int i = 0; i < containerContext->nb_streams; ++i)
			{
				InputStream* stream = inputStreams[i];
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
		InputStream* inputStream = inputStreams[streamIndex];

		if (inputStream != nullptr)
		{
			inputStream->DecodePacket(pkt);
		}

		// We need to unref the packet here because packets might pass by here
		// that don't have a stream attached to them. We want to dismiss them!
		av_packet_unref(pkt);
	}

	ContainerInfo Demuxer::GetInfo()
	{

		ContainerInfo info;

		// general data
		// the duration is calculated like this... why?
		int64_t duration = containerContext->duration + (containerContext->duration <= INT64_MAX - 5000 ? 5000 : 0);
		info.durationInMicroSeconds = duration;
		info.durationInSeconds = (float)info.durationInMicroSeconds / AV_TIME_BASE;
		info.start = (float)containerContext->start_time / AV_TIME_BASE;
		info.bitRate = containerContext->bit_rate;
		info.format = containerContext->iformat;


		// go over all streams and get their info
		for (int i = 0; i < containerContext->nb_streams; ++i)
		{
			InputStream* stream = GetInputStream(i);
			if (stream == nullptr) continue; // no valid stream
			stream->AddStreamInfo(&info);
		}

		return info;
	}

	int Demuxer::GetFrameCount(int streamId)
	{
		// Make sure all streams exist, so we can query them later.
		for (int i = 0; i < containerContext->nb_streams; ++i)
		{
			GetInputStream(i);
		}

		// Process the entire container so we can know how many frames are in each 
		while (!IsDone())
		{
			Step();
		}

		// Return the right stream's frame count.
		return GetInputStreamById(streamId)->GetFramesProcessed();
	}

	const char* Demuxer::GetFileName()
	{
		return fileName;
	}
}