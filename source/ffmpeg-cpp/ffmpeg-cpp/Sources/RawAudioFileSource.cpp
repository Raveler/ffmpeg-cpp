#include "RawAudioFileSource.h"
#include "FFmpegException.h"
#include "std.h"

using namespace std;

namespace ffmpegcpp
{
	RawAudioFileSource::RawAudioFileSource(const char* fileName, const char* inputFormat, int sampleRate, AudioFrameSink* frameSink)
	{

		// try to deduce the input format from the input format name
		AVInputFormat *file_iformat;
		if (!(file_iformat = av_find_input_format(inputFormat)))
		{
			CleanUp();
			throw FFmpegException("Unknown input format: " + string(inputFormat));
		}

		AVDictionary* format_opts = NULL;

		av_dict_set_int(&format_opts, "sample_rate", sampleRate, 0);

		// create the demuxer
		demuxer = new Demuxer(fileName, file_iformat, format_opts);
		demuxer->EncodeBestAudioStream(frameSink);
	}


	RawAudioFileSource::~RawAudioFileSource()
	{
		CleanUp();
	}

	void RawAudioFileSource::CleanUp()
	{
		if (demuxer != nullptr)
		{
			delete demuxer;
			demuxer = nullptr;
		}
	}

	void RawAudioFileSource::Start()
	{
		demuxer->Start();
	}
}

