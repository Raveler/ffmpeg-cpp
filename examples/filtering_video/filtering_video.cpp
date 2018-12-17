
#include <iostream>
#include <memory>

#include "ffmpegcpp.h"

using namespace ffmpegcpp;

int main()
{
	// This example will apply some filters to a video and write it back.
	try
	{
		// Create a muxer that will output the video as MKV.
		auto muxer = std::make_unique<Muxer>("filtered_video.mp4");

		// Create a MPEG2 codec that will encode the raw data.
		auto codec = std::make_unique<VideoCodec>(AV_CODEC_ID_MPEG2VIDEO);

		// Set the global quality of the video encoding. This maps to the command line
		// parameter -qscale and must be within range [0,31].
		codec->SetQualityScale(0);

		// Create an encoder that will encode the raw audio data as MP3.
		// Tie it to the muxer so it will be written to the file.
		auto encoder = std::make_unique<VideoEncoder>(codec.get(), muxer.get());

		// Create a video filter and do some funny stuff with the video data.
		auto filter = std::make_unique<VideoFilter>("scale=640:150,transpose=cclock,vignette", encoder.get());

		// Load a video from a container and send it to the filter first.
		auto demuxer = std::make_unique<Demuxer>("samples/big_buck_bunny.mp4");
		demuxer->DecodeBestVideoStream(filter.get());

		// Prepare the output pipeline. This will push a small amount of frames to the file sink until it IsPrimed returns true.
		demuxer->PreparePipeline();

		// Push all the remaining frames through.
		while (!demuxer->IsDone())
		{
			demuxer->Step();
		}
		
		// Save everything to disk by closing the muxer.
		muxer->Close();
	}
	catch (FFmpegException e)
	{
		std::cerr << "Exception caught!" << '\n';
		std::cerr << e.what() << '\n';
		throw e;
	}

	std::cout << "Encoding complete!" << '\n';
	std::cout << "Press any key to continue..." << '\n';

	getchar();
}
