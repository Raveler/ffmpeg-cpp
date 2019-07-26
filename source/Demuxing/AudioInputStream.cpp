#include "AudioInputStream.h"
#include "FFmpegException.h"
#include "CodecDeducer.h"

namespace ffmpegcpp
{
	AudioInputStream::AudioInputStream(AVFormatContext* format, AVStream* stream)
		: InputStream(format, stream)
	{
	}

	void AudioInputStream::ConfigureCodecContext()
	{

		// try to guess the channel layout for the decoder
		if (!codecContext->channel_layout)
		{
			codecContext->channel_layout = av_get_default_channel_layout(codecContext->channels);
		}
	}

	AudioInputStream::~AudioInputStream()
	{
	}

	void AudioInputStream::AddStreamInfo(ContainerInfo* containerInfo)
	{
		AudioStreamInfo info;

		info.id = stream->id; // the layout of the id's depends on the container format - it doesn't always start from 0 or 1!

		AVRational tb = stream->time_base;

		StreamData* metaData = new StreamData();
		info.timeBase = tb;

		AVCodecContext* codecContext = avcodec_alloc_context3(NULL);
		if (!codecContext) throw new FFmpegException("Failed to allocate temporary codec context.");
		int ret = avcodec_parameters_to_context(codecContext, stream->codecpar);
		if (ret < 0)
		{
			avcodec_free_context(&codecContext);
			throw new FFmpegException("Failed to read parameters from stream");
		}

		codecContext->properties = stream->codec->properties;
		codecContext->codec = stream->codec->codec;
		codecContext->qmin = stream->codec->qmin;
		codecContext->qmax = stream->codec->qmax;
		codecContext->coded_width = stream->codec->coded_width;
		codecContext->coded_height = stream->codec->coded_height;

		info.bitRate = CalculateBitRate(codecContext);

		AVCodec* codec = CodecDeducer::DeduceDecoder(codecContext->codec_id);
		info.codec = codec;

		info.sampleRate = codecContext->sample_rate;
		info.channels = codecContext->channels;
		info.channelLayout = codecContext->channel_layout;
		av_get_channel_layout_string(info.channelLayoutName, 255, codecContext->channels, codecContext->channel_layout);

		avcodec_free_context(&codecContext);

		containerInfo->audioStreams.push_back(info);
	}
}



