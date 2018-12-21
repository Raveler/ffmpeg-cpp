#pragma once

#include "ffmpeg.h"

#include <exception>
#include <string>

namespace ffmpegcpp
{
	class FFmpegException : std::exception
	{

	public:

		FFmpegException(const std::string & error);

		FFmpegException(const std::string & error, int returnValue);

		char const* what() const override
		{
			return std::exception::what();
		}

	private:

		char error[AV_ERROR_MAX_STRING_SIZE];
	};
}