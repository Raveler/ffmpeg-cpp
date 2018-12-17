
#include <iostream>
#include <memory>

#include "ffmpegcpp.h"

int main()
{
	// This example will take a raw audio file and encode it into as MP3.
	try
	{
		// Create a muxer that will output as MP3.
		auto muxer = std::make_unique<ffmpegcpp::Muxer>("output.mp3");

		// Create a MP3 codec that will encode the raw data.
		auto codec = std::make_unique<ffmpegcpp::AudioCodec>(AV_CODEC_ID_MP3);

		// Create an encoder that will encode the raw audio data as MP3.
		// Tie it to the muxer so it will be written to the file.
		auto encoder = std::make_unique<ffmpegcpp::AudioEncoder>(codec.get(), muxer.get());

		// Load the raw audio file so we can process it.
		// We need to provide some info because we can't derive it from the raw format.
		// Hand it the encoder so it will pass on its raw data to the encoder, which will in turn pass it on to the muxer.
		const char* rawAudioFile = "samples/Vivaldi_s16le_2_channels_samplerate_11025.dat";
		const char* rawAudioFormat = "s16le"; int rawAudioSampleRate = 11025; int rawAudioChannels = 2;
		auto audioFile = std::make_unique<ffmpegcpp::RawAudioFileSource>(rawAudioFile, rawAudioFormat, rawAudioSampleRate, rawAudioChannels, encoder.get());

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
	catch (ffmpegcpp::FFmpegException e)
	{
		std::cerr << "Exception caught!" << '\n';
		std::cerr << e.what() << '\n';
		throw e;
	}

	std::cout << "Encoding complete!" << '\n';
	std::cout << "Press any key to continue..." << '\n';

	getchar();
}
