#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	FFmpegException::FFmpegException(const string & error) : exception(error.c_str())
	{
	}

	FFmpegException::FFmpegException(const string & error, int returnValue)
		: exception(
			(error + ": " + av_make_error_string(this->error, AV_ERROR_MAX_STRING_SIZE, returnValue)).c_str()
		)
	{
	}
}