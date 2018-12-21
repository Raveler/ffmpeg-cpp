#include "Codecs/AudioCodec.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	AudioCodec::AudioCodec(const char* codecName)
		: Codec(codecName)
	{
	}

	AudioCodec::AudioCodec(AVCodecID codecId)
		: Codec(codecId)
	{
	}

	AudioCodec::~AudioCodec()
	{
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

	bool AudioCodec::IsChannelsSupported(int channels)
	{
		int64_t channelLayout = av_get_default_channel_layout(channels);
		const uint64_t *p = codecContext->codec->channel_layouts;
		while (*p)
		{
			if (channelLayout == *p) return true;
			p++;
		}
		return false;
	}

	bool AudioCodec::IsFormatSupported(AVSampleFormat format)
	{
		const enum AVSampleFormat *p = codecContext->codec->sample_fmts;

		while (*p != AV_SAMPLE_FMT_NONE)
		{
			if (*p == format) return true;
			p++;
		}
		return false;
	}

	bool AudioCodec::IsSampleRateSupported(int sampleRate)
	{
		const int *p;
		if (!codecContext->codec->supported_samplerates) return true; // all sample rates are fair game
		p = codecContext->codec->supported_samplerates;
		while (*p)
		{
			if (*p == sampleRate) return true;
			p++;
		}
		return false;
	}

	AVSampleFormat AudioCodec::GetDefaultSampleFormat()
	{
		AVSampleFormat format = (codecContext->codec->sample_fmts ? codecContext->codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP);
		return format;
	}

	int AudioCodec::GetDefaultSampleRate()
	{
		return select_sample_rate(codecContext->codec);
	}

	OpenCodec* AudioCodec::Open(int bitRate, AVSampleFormat format, int sampleRate)
	{

		// do some sanity checks
		if (!IsFormatSupported(format)) throw FFmpegException("Sample format " + string(av_get_sample_fmt_name(format)) + " is not supported by codec " + codecContext->codec->name);
		if (!IsSampleRateSupported(sampleRate)) throw FFmpegException("Sample rate " + to_string(sampleRate) + " is not supported by codec " + codecContext->codec->name);

		// if the codec is not an audio codec, we are doing it wrong!
		if (codecContext->codec->type != AVMEDIA_TYPE_AUDIO) throw FFmpegException("An audio output stream must be initialized with an audio codec");

		// set all data
		codecContext->bit_rate = bitRate;
		codecContext->sample_fmt = format;
		codecContext->sample_rate = sampleRate;

		// deduce the best channel layout from the codec
		codecContext->channel_layout = select_channel_layout(codecContext->codec);

		// finally the number of channels is derived from the layout
		codecContext->channels = av_get_channel_layout_nb_channels(codecContext->channel_layout);

		// default flags
		codecContext->flags = 0;

		// open
		return Codec::Open();
	}
}
