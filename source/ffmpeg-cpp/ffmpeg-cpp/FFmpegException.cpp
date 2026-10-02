#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	FFmpegException::FFmpegException(const string &error) : exception(),errorMsg(error)
	{
	}

	FFmpegException::FFmpegException(const string &errorStr, int returnValue)
		:exception()
	{
		 char error[AV_ERROR_MAX_STRING_SIZE];
		 errorMsg=errorStr + ": " + av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, returnValue);
	}
}
