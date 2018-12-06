#include "RawVideoFileSource.h"
#include "FFmpegException.h"
#include "std.h"

using namespace std;

namespace ffmpegcpp
{
	RawVideoFileSource::RawVideoFileSource(const char* fileName, VideoFrameSink* frameSink)
	{

		// create the demuxer - it can handle figuring out the video type on its own apparently
		try
		{
			demuxer = new Demuxer(fileName, NULL, NULL);
			demuxer->EncodeBestVideoStream(frameSink);
		}
		catch (FFmpegException e)
		{
			CleanUp();
			throw e;
		}
	}


	RawVideoFileSource::~RawVideoFileSource()
	{
		CleanUp();
	}

	void RawVideoFileSource::CleanUp()
	{
		if (demuxer != nullptr)
		{
			delete demuxer;
			demuxer = nullptr;
		}
	}

	void RawVideoFileSource::Start()
	{
		demuxer->Start();
	}
}

