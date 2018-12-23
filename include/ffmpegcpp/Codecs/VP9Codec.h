#pragma once
#include "VideoCodec.h"

#include <string>

namespace ffmpegcpp
{

	class VP9Codec : public VideoCodec
	{

	public:

		VP9Codec();

		void SetDeadline(const std::string & deadline);
		void SetCpuUsed(int cpuUsed);

		void SetLossless(bool lossless);
		void SetCrf(int crf);
	};

}