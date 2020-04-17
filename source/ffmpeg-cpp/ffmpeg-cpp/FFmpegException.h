#pragma once

#include "ffmpeg-cpp/ffmpeg.h"



namespace ffmpegcpp
{
	class FFmpegException : std::exception
	{

	public:

		FFmpegException(std::string error);

		FFmpegException(std::string error, int returnValue);

//		virtual const char* what() const
//		{
//			return std::exception::what();
//		}


	private:

		char error[AV_ERROR_MAX_STRING_SIZE];
	};
}
