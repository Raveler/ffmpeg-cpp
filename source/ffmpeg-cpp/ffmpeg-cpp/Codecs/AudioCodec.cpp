#include "AudioCodec.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	AudioCodec::AudioCodec(const char* codecName, int bitRate, AVSampleFormat format)
		: Codec(codecName)
	{
		InitContext(bitRate, format);
	}

	AudioCodec::AudioCodec(AVCodecID codecId, int bitRate, AVSampleFormat format)
		: Codec(codecId)
	{
		InitContext(bitRate, format);
	}

	AudioCodec::AudioCodec(const char* codecName)
		: Codec(codecName)
	{
		InitContextWithDefaults();
	}

	AudioCodec::AudioCodec(AVCodecID codecId)
		: Codec(codecId)
	{
		InitContextWithDefaults();
	}

	void AudioCodec::InitContextWithDefaults()
	{
		AVSampleFormat format = (codecContext->codec->sample_fmts ? codecContext->codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP);
		int bitRate = 64000;
		InitContext(bitRate, format);
	}

	/* check that a given sample format is supported by the encoder */
	static int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
	{
		const enum AVSampleFormat *p = codec->sample_fmts;

		while (*p != AV_SAMPLE_FMT_NONE)
		{
			if (*p == sample_fmt)
				return 1;
			p++;
		}
		return 0;
	}

	// calculate the best sample rate for a codec, defaults to 44100
	static int select_sample_rate(const AVCodec *codec)
	{
		const int *p;
		int best_samplerate = 0;

		if (!codec->supported_samplerates)
			return 44100;

		p = codec->supported_samplerates;
		while (*p)
		{
			if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
				best_samplerate = *p;
			p++;
		}
		return best_samplerate;
	}

	/* select layout with the highest channel count */
	static uint64_t select_channel_layout(const AVCodec *codec)
	{
		const uint64_t *p;
		uint64_t best_ch_layout = 0;
		int best_nb_channels = 0;

		if (!codec->channel_layouts)
			return AV_CH_LAYOUT_STEREO;

		p = codec->channel_layouts;
		while (*p)
		{
			int nb_channels = av_get_channel_layout_nb_channels(*p);

			if (nb_channels > best_nb_channels)
			{
				best_ch_layout = *p;
				best_nb_channels = nb_channels;
			}
			p++;
		}
		return best_ch_layout;
	}

	void AudioCodec::InitContext(int bitRate, AVSampleFormat format)
	{

		// basic settings
		codecContext->bit_rate = bitRate;
		codecContext->sample_fmt = format;

		// verify if the format is supported
		if (!check_sample_fmt(codecContext->codec, codecContext->sample_fmt))
		{
			throw FFmpegException("Encoder does not support sample format " + string(av_get_sample_fmt_name(codecContext->sample_fmt)));
		}

		// deduce the sample rate from the codec
		codecContext->sample_rate = select_sample_rate(codecContext->codec);

		// deduce the best channel layout from the codec
		codecContext->channel_layout = select_channel_layout(codecContext->codec);

		// finally the number of channels is derived from the layout
		codecContext->channels = av_get_channel_layout_nb_channels(codecContext->channel_layout);

		codecContext->flags = 0;
	}
}
