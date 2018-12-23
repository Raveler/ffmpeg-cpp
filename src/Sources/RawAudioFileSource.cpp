#include "Sources/RawAudioFileSource.h"

#include "ffmpeg.h"
#include "FFmpegException.h"
#include "Sources/Demuxer.h"

using namespace std;

namespace ffmpegcpp
{
	RawAudioFileSource::RawAudioFileSource(const std::string & fileName, const std::string & inputFormat, int sampleRate, int channels, AudioFrameSink* frameSink)
	{

		// try to deduce the input format from the input format name
		AVInputFormat *file_iformat;
		if (!(file_iformat = av_find_input_format(inputFormat.c_str())))
		{
			throw FFmpegException("Unknown input format: " + inputFormat);
		}

		AVDictionary* format_opts = nullptr;

		av_dict_set_int(&format_opts, "sample_rate", sampleRate, 0);
		av_dict_set_int(&format_opts, "channels", channels, 0);

		// create the demuxer
		try
		{
			demuxer = std::make_unique<Demuxer>(fileName, file_iformat, format_opts);
			demuxer->DecodeBestAudioStream(frameSink);
		}
		catch (FFmpegException e)
		{
			throw e;
		}
	}

	void RawAudioFileSource::PreparePipeline()
	{
		demuxer->PreparePipeline();
	}

	bool RawAudioFileSource::IsDone() const
	{
		return demuxer->IsDone();
	}

	void RawAudioFileSource::Step()
	{
		demuxer->Step();
	}
}

