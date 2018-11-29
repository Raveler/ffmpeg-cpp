#pragma once

#include "ffmpeg.h"

#include "std.h"

namespace ffmpegcpp
{
	class FFmpegException : exception
	{

	public:

		FFmpegException(string error);

		FFmpegException(string error, int returnValue);

		virtual char const* what() const
		{
			return exception::what();
		}


	private:

		char error[AV_ERROR_MAX_STRING_SIZE];
	};
}