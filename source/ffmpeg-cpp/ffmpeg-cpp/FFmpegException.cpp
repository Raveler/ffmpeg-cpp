#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	FFmpegException::FFmpegException(string error) : errormsg( error )
	{
	}

	FFmpegException::FFmpegException(string error, int returnValue)
	{
		char av_error[AV_ERROR_MAX_STRING_SIZE];
		errormsg = error + ": " + av_make_error_string(av_error, AV_ERROR_MAX_STRING_SIZE, returnValue);
	}
}