// MathLibrary.h - Contains declarations of math functions
#pragma once

#define DllExport __declspec(dllexport)


extern "C" DllExport void* ffmpegCppCreate(const char* outputFileName);

extern "C" DllExport void ffmpegCppAddVideoStream(void* handle, const char* videoFileName);
extern "C" DllExport void ffmpegCppAddAudioStream(void* handle, const char* audioFileName);

extern "C" DllExport void ffmpegCppAddVideoFilter(void* handle, const char* filterString);
extern "C" DllExport void ffmpegCppAddAudioFilter(void* handle, const char* filterString);

extern "C" DllExport void ffmpegCppGenerate(void* handle);

extern "C" DllExport bool ffmpegCppIsError(void* handle);
extern "C" DllExport const char* ffmpegCppGetError(void* handle);

extern "C" DllExport void ffmpegCppClose(void* handle);