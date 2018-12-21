#pragma once

#include "FFmpegResource.h"

#include <string>
#include <vector>

struct AVFormatContext;
struct AVOutputFormat;
struct AVPacket;

namespace ffmpegcpp {

	class OutputStream;

	class Muxer
	{
	public:

		Muxer(const std::string & fileName);
		~Muxer();

		void AddOutputStream(OutputStream* stream);

		void WritePacket(AVPacket* pkt);

		void Close();
		
		bool IsPrimed(); // ToDo: this should be const...


	private:

		void Open();
		
		std::vector<OutputStream*> outputStreams;
		std::vector<FFmpegResource<AVPacket>> packetQueue;

		AVOutputFormat* containerFormat;

		AVFormatContext* containerContext = nullptr;

		std::string fileName;

		void CleanUp();

		bool opened = false;
	};
}
