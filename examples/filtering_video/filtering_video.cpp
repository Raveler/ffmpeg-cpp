
#include <iostream>

#include "ffmpegcpp.h"

using namespace std;
using namespace ffmpegcpp;

int main()
{
	// This example will apply some filters to a video and write it back.
	try
	{
		// Create a muxer that will output the video as MKV.
		Muxer* muxer = new Muxer("filtered_video.mp4");

		// Create a MPEG2 codec that will encode the raw data.
		VideoCodec* codec = new VideoCodec(AV_CODEC_ID_MPEG2VIDEO);

		// Set the global quality of the video encoding. This maps to the command line
		// parameter -qscale and must be within range [0,31].
		codec->SetQualityScale(0);

		// Create an encoder that will encode the raw audio data as MP3.
		// Tie it to the muxer so it will be written to the file.
		VideoEncoder* encoder = new VideoEncoder(codec, muxer);

		// Create a video filter and do some funny stuff with the video data.
		Filter* filter = new Filter("scale=640:150,transpose=cclock,vignette", encoder);

		// Load a video from a container and send it to the filter first.
		Demuxer* demuxer = new Demuxer("samples/big_buck_bunny.mp4");
		demuxer->DecodeBestVideoStream(filter);

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
		cerr << "Exception caught!" << endl;
		cerr << e.what() << endl;
		throw e;
	}

	cout << "Encoding complete!" << endl;
	cout << "Press any key to continue..." << endl;

	getchar();
}
