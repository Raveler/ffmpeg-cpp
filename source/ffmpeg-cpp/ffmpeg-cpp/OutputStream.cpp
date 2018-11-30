#include "OutputStream.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	OutputStream::OutputStream(OpenCodec* codec)
	{
		this->codec = codec;
	}

	OpenCodec* OutputStream::GetCodec()
	{
		return codec;
	}
}
