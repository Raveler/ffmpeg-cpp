#include "OneInputFrameSink.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{

	OneInputFrameSink::OneInputFrameSink(FrameWriter* writer)
	{
		this->writer = writer;
	}

	FrameSinkStream* OneInputFrameSink::CreateStream()
	{
		++nStreamsGenerated;
		if (nStreamsGenerated > 1)
		{
			throw new FFmpegException("This frame sink only supports one input");
		}
		stream = new FrameSinkStream(writer, 0);
		return stream;
	}

	OneInputFrameSink::~OneInputFrameSink()
	{
		if (stream != nullptr) delete stream;
	}
}

