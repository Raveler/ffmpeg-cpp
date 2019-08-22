# ffmpeg-cpp
A clean C++ wrapper around the ffmpeg libraries which can be used in any C++ project or C# project (with DllImport or CLR). Makes the most commonly used functionality of FFmpeg easily available for any C++ projects with an easy-to-use interface. The full power of FFmpeg compacted in 10 lines of C++ code: if this sounds useful to you, read on!

# Installation

## Windows

1. Clone the repository
2. Download a build of FFmpeg from https://ffmpeg.zeranoe.com/builds/. The project was last tested with **4.1**. You will need both the dev version (for .h and .lib files) and the shared version (for .dll). Extract all of them into the ffmpeg directory in the repository. there are more instructions on how to extract them properly in the ffmpeg/readme.txt file.
3. Open the Visual Studio solution in the source directory.
4. Build everything.

This will build a .lib file that you can use in your own C++ projects. It will also generate an include-folder in the repo root that you can add to your include filders in your own project.

## Linux

Currently, only a Windows environment with Visual Studio is supported. This is simply because I do not have experience with cmake and Linux-projects, since the project itself is fully platform-independent. If anyone is willing to help me out with configuring cmake, please get in touch!

# Usage

There are multiple demo projects included in the solution. Check out the demo-project for a thorough exploration of the features (demuxing, decoding, filtering, encoding, muxing) or one of the other examples for a simpler example to follow.

There is also a .NET Core compatible simplified interface included so that you can embed this project in your .NET Core projects.

## C++

To give you an idea, this code will load a video stream from a container, filter it, and write it back out to another container:

```C++
// Create a muxer that will output the video as MP4.
Muxer* muxer = new Muxer("filtered_video.mp4");

// Create a MPEG2 codec that will encode the raw data.
VideoCodec* codec = new VideoCodec(AV_CODEC_ID_MPEG2VIDEO);

// Create an encoder that will encode the raw audio data using the codec specified above.
// Tie it to the muxer so it will be written to file.
VideoEncoder* encoder = new VideoEncoder(codec, muxer);

// Create a video filter and do some funny stuff with the video data.
Filter* filter = new Filter("scale=640:150,transpose=cclock,vignette", encoder);

// Load a container. Pick the best video stream container in the container
// And send it to the filter.
Demuxer* demuxer = new Demuxer("big_buck_bunny.mp4");
demuxer->DecodeBestVideoStream(filter);

// Prepare the output pipeline.
// This will decode a small amount of frames so the pipeline can configure itself.
demuxer->PreparePipeline();

// Push all the remaining frames through.
while (!demuxer->IsDone())
{
	demuxer->Step();
}
		
// Save everything to disk by closing the muxer.
muxer->Close();
```

If you use the included simple-interface library, which only supports a subset of the full library, using ffmpeg-cpp becomes even easier:

```
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
```

## C\#

The simple-interface is made in such a way that it can easily be called using [DllImport] from any C# project:

```
	public class Example
	{

		public Example()
		{
			try
			{
				string inFileName = "in.mp4";
				string outFileName = "out.mp4";
				IntPtr handle = ffmpegCppCreate(outFileName); CheckError(handle);
				ffmpegCppAddVideoStream(handle, inFileName); CheckError(handle);
				ffmpegCppAddAudioStream(handle, inFileName); CheckError(handle);
				ffmpegCppAddVideoFilter(handle, "crop=1080:1920:740:0[middle];[middle]transpose=3"); CheckError(handle);
				ffmpegCppGenerate(handle); CheckError(handle);
				ffmpegCppClose(handle);
			}
			catch (InvalidOperationException e)
			{
				Console.WriteLine("ERROR: " + e.Message);
			}
		}

		private void CheckError(IntPtr handle)
		{
			if (ffmpegCppIsError(handle))
			{
				IntPtr errorPtr = ffmpegCppGetError(handle);
				string error = Marshal.PtrToStringAnsi(errorPtr);
				throw new InvalidOperationException(error);
			}
		}

		[DllImport("simple_interface.dll", CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr ffmpegCppCreate(string outFileName);

		[DllImport("simple_interface.dll", CallingConvention = CallingConvention.Cdecl)]
		private static extern void ffmpegCppAddVideoStream(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] string inFileName);

		[DllImport("simple_interface.dll", CallingConvention = CallingConvention.Cdecl)]
		private static extern void ffmpegCppAddAudioStream(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] string inFileName);

		[DllImport("simple_interface.dll", CallingConvention = CallingConvention.Cdecl)]
		private static extern void ffmpegCppAddVideoFilter(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] string filterString);

		[DllImport("simple_interface.dll", CallingConvention = CallingConvention.Cdecl)]
		private static extern void ffmpegCppAddAudioFilter(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] string filterString);

		[DllImport("simple_interface.dll", CallingConvention = CallingConvention.Cdecl)]
		private static extern void ffmpegCppGenerate(IntPtr handle);

		[DllImport("simple_interface.dll", CallingConvention = CallingConvention.Cdecl)]
		private static extern bool ffmpegCppIsError(IntPtr handle);

		[DllImport("simple_interface.dll", CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr ffmpegCppGetError(IntPtr handle);

		[DllImport("simple_interface.dll", CallingConvention = CallingConvention.Cdecl)]
		private static extern void ffmpegCppClose(IntPtr handle);
	}
```

If you want to use ffmpeg-cpp in a C# project, you can easily do so by making your own C-wrapper around the 

# Why?

I developed this project to be able to to integrate FFmpeg into our program without having to call the executable to do an operation. This is important because starting up an external executable tends to be blocked by antivirus software and can cause issues with users. It has been tested for the most common functionality, and some of the examples from https://github.com/FFmpeg/FFmpeg/tree/master/doc/examples are mirrored in the project as well.

# Roadmap

- Add Linux/Mac build support
- Adding proper unit tests
- Testing with more codecs, containers

# License

This library is licensed under LGPL (https://en.wikipedia.org/wiki/GNU_Lesser_General_Public_License).

Please note though that FFmpeg, which you will need to build this library, is not. Depending on how you build it, it is either LGPL or GPL. So if you use the GPL-version of FFmpeg in your project, this library will be GPL too.
