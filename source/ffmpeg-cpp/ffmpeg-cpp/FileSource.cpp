#include "FileSource.h"


namespace ffmpegcpp
{
	FileSource::FileSource(const char* inFileName, OutputStream* output)
	{
		buffer = new uint8_t[bufferSize];

		/* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
		memset(buffer + (int)bufferSize, 0, AV_INPUT_BUFFER_PADDING_SIZE);
	}


	FileSource::~FileSource()
	{
		delete buffer;
	}
}
