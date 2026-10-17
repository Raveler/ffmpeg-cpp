#pragma once

#include "ffmpeg.h"

#include "std.h"

namespace ffmpegcpp
{
	class FFmpegException : std::exception
	{

	public:

		FFmpegException(std::string error);

		FFmpegException(std::string error, int returnValue);

		virtual char const* what() const noexcept
		{
			return errormsg.c_str();
		}


	private:
		std::string errormsg;
	};
}