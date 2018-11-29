#pragma once

#include "ffmpeg.h"
#include "OutputStream.h"

namespace ffmpegcpp
{
	class FileSource
	{

	public:
		FileSource(const char* inFileName, OutputStream* output);
		~FileSource();


	private:

		const float bufferSize = 4096;

		uint8_t* buffer;
	};
}
