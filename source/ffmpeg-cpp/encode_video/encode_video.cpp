
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
		Muxer* muxer = new Muxer("output.mpg");

		// Create a MPEG2 codec that will encode the raw data.
		VideoCodec* codec = new VideoCodec("mpeg2video");

		// Set the global quality of the video encoding. This maps to the command line
		// parameter -qscale and must be within range [0,31].
		codec->SetQualityScale(0);

		// Set the bit rate option -b:v 2M
		codec->SetGenericOption("b", "2M");

		// Create an encoder that will encode the raw audio data as MP3.
		// Tie it to the muxer so it will be written to the file.
		VideoEncoder* encoder = new VideoEncoder(codec, muxer);

		// Load the raw video file so we can process it.
		// FFmpeg is very good at deducing the file format, even from raw video files,
		// but if we have something weird, we can specify the properties of the format
		// in the constructor as commented out below.
		RawVideoFileSource* videoFile = new RawVideoFileSource("samples/carphone_qcif.y4m", encoder);

		// Prepare the output pipeline. This will push a small amount of frames to the file sink until it IsPrimed returns true.
		videoFile->PreparePipeline();

		// Push all the remaining frames through.
		while (!videoFile->IsDone())
		{
			videoFile->Step();
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
