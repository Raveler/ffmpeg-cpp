
#include <iostream>

#include "ffmpegcpp.h"

using namespace std;
using namespace ffmpegcpp;

int main()
{
	// This example will take a raw audio file and encode it into as MP3.
	try
	{
		// Create a muxer that will output as MP3.
		Muxer* muxer = new Muxer("output.mp3");

		// Create a MP3 codec that will encode the raw data.
		AudioCodec* codec = new AudioCodec(AV_CODEC_ID_MP3);

		// Create an encoder that will encode the raw audio data as MP3.
		// Tie it to the muxer so it will be written to the file.
		AudioEncoder* encoder = new AudioEncoder(codec, muxer);

		// Load the raw audio file so we can process it.
		// We need to provide some info because we can't derive it from the raw format.
		// Hand it the encoder so it will pass on its raw data to the encoder, which will in turn pass it on to the muxer.
		const char* rawAudioFile = "samples/Vivaldi_s16le_2_channels_samplerate_11025.dat";
		const char* rawAudioFormat = "s16le"; int rawAudioSampleRate = 11025; int rawAudioChannels = 2;
		RawAudioFileSource* audioFile = new RawAudioFileSource(rawAudioFile, rawAudioFormat, rawAudioSampleRate, rawAudioChannels, encoder);

		// Prepare the output pipeline. This will push a small amount of frames to the file sink until it IsPrimed returns true.
		audioFile->PreparePipeline();

		// Push all the remaining frames through.
		while (!audioFile->IsDone())
		{
			audioFile->Step();
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
