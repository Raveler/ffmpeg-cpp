#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "OutputStream.h"

namespace ffmpegcpp {

	class Muxer
	{
	public:

		Muxer(const char* fileName, std::vector<OutputStream*> streams);
		~Muxer();

		void Close();

	private:

		void Open();

		std::vector<OutputStream*> streams;

		AVOutputFormat* containerFormat;

		AVFormatContext* containerContext = nullptr;

		std::string fileName;

		void CleanUp();
	};
}
