
#include <iostream>
#include <memory>

#include "ffmpegcpp.h"

using namespace ffmpegcpp;


int main()
{
	// This example will take a raw audio file and encode it into as MP3.
	try
	{
		// Create a muxer that will output the video as MKV.
		auto muxer = std::make_unique<Muxer>("output.mkv");

		// Create a codec that will encode video as VP9
		auto videoCodec = std::make_unique<VP9Codec>();

		// Configure the codec to not do compression, to use multiple CPU's and to go as fast as possible.
		videoCodec->SetLossless(true);
		videoCodec->SetCpuUsed(5);
		videoCodec->SetDeadline("realtime");

		// Create a codec that will encode audio as AAC
		auto audioCodec = std::make_unique<AudioCodec>(AV_CODEC_ID_AAC);

		// Create encoders for both
		auto videoEncoder = std::make_unique<VideoEncoder>(videoCodec.get(), muxer.get());
		auto audioEncoder = std::make_unique<AudioEncoder>(audioCodec.get(), muxer.get());

		// Load both audio and video from a container
		auto videoContainer = std::make_unique<Demuxer>("samples/big_buck_bunny.mp4");
		auto audioContainer = std::make_unique<Demuxer>("samples/DesiJourney.wav");

		// Tie the best stream from each container to the output
		videoContainer->DecodeBestVideoStream(videoEncoder.get());
		audioContainer->DecodeBestAudioStream(audioEncoder.get());

		// Prepare the pipeline. We want to call this before the rest of the loop
		// to ensure that the muxer will be fully ready to receive data from
		// multiple sources.
		videoContainer->PreparePipeline();
		audioContainer->PreparePipeline();

		// Pump the audio and video fully through.
		// To avoid big buffers, we interleave these calls so that the container
		// can be written to disk efficiently.
		while (!videoContainer->IsDone() || !audioContainer->IsDone())
		{
			if (!videoContainer->IsDone()) videoContainer->Step();
			if (!audioContainer->IsDone()) audioContainer->Step();
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
