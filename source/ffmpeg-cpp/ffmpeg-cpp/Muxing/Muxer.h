#pragma once

#include "ffmpeg.h"
#include "std.h"


namespace ffmpegcpp {

	class OutputStream;

	class Muxer
	{
	public:

		Muxer(const char* fileName);
		~Muxer();

		void AddOutputStream(OutputStream* stream);

		void WritePacket(AVPacket* pkt);

		void Close();


	private:

		void Open();

		std::vector<OutputStream*> streams;

		AVOutputFormat* containerFormat;

		AVFormatContext* containerContext = nullptr;

		std::string fileName;

		void CleanUp();

		bool opened = false;
	};
}
