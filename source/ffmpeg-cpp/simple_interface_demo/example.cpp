
#include "SimpleInterface.h"

int main()
{
	void* handle = ffmpegCppCreate("out.mp4");
	ffmpegCppAddVideoStream(handle, "samples/big_buck_bunny.mp4");
	ffmpegCppAddVideoFilter(handle, "transpose=cclock[middle];[middle]vignette");
	ffmpegCppAddAudioStream(handle, "samples/big_buck_bunny.mp4");
	ffmpegCppGenerate(handle);
	ffmpegCppClose(handle);
}
