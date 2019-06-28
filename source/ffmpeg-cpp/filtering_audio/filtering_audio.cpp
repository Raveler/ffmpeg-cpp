
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
		Muxer* muxer = new Muxer("Vivaldi_filtered.aac");

		// Create an MP3 codec that will encode the raw data.
		AudioCodec* codec = new AudioCodec(AV_CODEC_ID_AAC);

		// Create an encoder that will encode the raw audio data as MP3.
		// Tie it to the muxer so it will be written to the file.
		AudioEncoder* encoder = new AudioEncoder(codec, muxer);

		// Create a video filter and do some funny stuff with the video data.
		Filter* filter = new Filter("areverse", encoder);

		// THIS COMMENTED EXAMPLE BELOW SHOWS A FILTER FROM AUDIO TO PNG
		// It will render the waveform of the audio.

		// Create a muxer that will output as PNG.
		/*Muxer* muxer = new Muxer("Vivaldi_waveform.png");

		// Create a MPEG2 codec that will encode the raw data.
		VideoCodec* codec = new PNGCodec();

		// Create an encoder that will encode the raw audio data as MP3.
		// Tie it to the muxer so it will be written to the file.
		VideoEncoder* encoder = new VideoEncoder(codec, muxer);

		Filter* filter = new Filter("showwavespic=colors=white:s=512x64", encoder); */

		// Load a video from a container and send it to the filter first.
		Demuxer* demuxer = new Demuxer("samples/Vivaldi_Sonata_eminor_.mp3");
		demuxer->DecodeBestAudioStream(filter);

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
