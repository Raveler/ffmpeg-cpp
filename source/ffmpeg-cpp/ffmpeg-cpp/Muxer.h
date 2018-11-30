#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "OutputStream.h"

namespace ffmpegcpp {

	class Muxer
	{
	public:

		Muxer(const char* fileName, std::vector<OutputStream*> streams);

		void Close();

	private:

		void Open();

		std::vector<OutputStream*> streams;

		AVFormatContext* containerContext;
		AVOutputFormat* containerFormat;

		std::string fileName;
	};
}
