
#include <iostream>

#include "ffmpegcpp.h"

using namespace std;
using namespace ffmpegcpp;


int main()
{
	// This example will take a raw audio file and encode it into as MP3.
	try
	{
		// Create a muxer that will output the video as MKV.
		Muxer* muxer = new Muxer("output.mkv");

		// Create a codec that will encode video as VP9
		VP9Codec* videoCodec = new VP9Codec();

		// Configure the codec to not do compression, to use multiple CPU's and to go as fast as possible.
		videoCodec->SetLossless(true);
		videoCodec->SetCpuUsed(5);
		videoCodec->SetDeadline("realtime");

		// Create a codec that will encode audio as AAC
		AudioCodec* audioCodec = new AudioCodec(AV_CODEC_ID_AAC);

		// Create encoders for both
		VideoEncoder* videoEncoder = new VideoEncoder(videoCodec, muxer);
		AudioEncoder* audioEncoder = new AudioEncoder(audioCodec, muxer);

		// Load both audio and video from a container
		Demuxer* videoContainer = new Demuxer("samples/big_buck_bunny.mp4");
		Demuxer* audioContainer = new Demuxer("samples/DesiJourney.wav");

		// Tie the best stream from each container to the output
		videoContainer->DecodeBestVideoStream(videoEncoder);
		audioContainer->DecodeBestAudioStream(audioEncoder);

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
		cerr << "Exception caught!" << endl;
		cerr << e.what() << endl;
		throw e;
	}

	cout << "Encoding complete!" << endl;
	cout << "Press any key to continue..." << endl;

	getchar();
}
