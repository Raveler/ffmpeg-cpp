#include "OutputStream.h"
#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	OutputStream::OutputStream(Muxer* muxer, Codec* codec)
	{
		this->muxer = muxer;
		this->codec = codec;
	}
}
