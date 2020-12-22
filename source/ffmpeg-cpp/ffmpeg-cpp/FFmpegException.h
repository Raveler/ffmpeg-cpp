#pragma once

#include "ffmpeg.h"

#include "std.h"

namespace ffmpegcpp
{
	class FFmpegException : std::exception
	{

	public:

		FFmpegException(const std::string &error);

		FFmpegException(const std::string &error, int returnValue);

		char const* what() const  noexcept override
		{
			return errorMsg.c_str();
		}


	private:

		std::string errorMsg;
	};
}
