
#include <iostream>

#include "ffmpegcpp.h"

using namespace std;
using namespace ffmpegcpp;


int main()
{
	// This example will apply a vignette filter and then compare the changed output to
	// the original video by using the difference filter.
	try
	{
		// Create a muxer that will output the video as MKV.
		Muxer* muxer = new Muxer("out.mp4");

		// Create a codec that will encode video as VP9
		PNGCodec* videoCodec = new PNGCodec();

		// Create encoders for both
		VideoEncoder* videoEncoder = new VideoEncoder(videoCodec, muxer);

		// Create a video filter and do some funny stuff with the video data.
		Filter* filter = new Filter("blend=all_mode=difference", videoEncoder);
		//VideoFilter* filter = new VideoFilter("overlay=0:0", videoEncoder);
		//VideoFilter* filter = new VideoFilter("scale=100:800", videoEncoder);

		// Create a video filter that will put a vignette on one of the video's,
		// so that our difference filter van detect this.
		Filter* vignetteFilter = new Filter("vignette", filter);

		// Load both video's
		Demuxer* videoContainer1 = new Demuxer("samples/carphone.h264");
		Demuxer* videoContainer2 = new Demuxer("samples/carphone.h264");

		// Tie the best stream from each container to the output
		videoContainer1->DecodeBestVideoStream(filter);
		videoContainer2->DecodeBestVideoStream(vignetteFilter);

		// Prepare the pipeline. We want to call this before the rest of the loop
		// to ensure that the muxer will be fully ready to receive data from
		// multiple sources.
		videoContainer1->PreparePipeline();
		videoContainer2->PreparePipeline();

		// Pump the audio and video fully through.
		// To avoid big buffers, we interleave these calls so that the container
		// can be written to disk efficiently.
		while (!videoContainer1->IsDone() || !videoContainer2->IsDone())
		{
			if (!videoContainer1->IsDone()) videoContainer1->Step();
			if (!videoContainer2->IsDone()) videoContainer2->Step();
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
