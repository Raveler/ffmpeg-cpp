
#include <iostream>
#include <memory>

#include "ffmpegcpp.h"

class RawAudioFileSink : public ffmpegcpp::AudioFrameSink
{
public:

	RawAudioFileSink(const std::string & fileName)
	{
		file = fopen(fileName.c_str(), "wb");
	}

	void WriteFrame(AVFrame* frame, AVRational* timeBase) override
	{
		// Just write out the samples channel by channel to a file.
		int data_size = av_get_bytes_per_sample((AVSampleFormat)frame->format);
		for (int i = 0; i < frame->nb_samples; i++)
		{
			for (int ch = 0; ch < frame->channels; ch++)
			{
				fwrite(frame->data[ch] + data_size * i, 1, data_size, file);
			}
		}
	}

	void Close() override
	{
		fclose(file);
	}

	bool IsPrimed() const override
	{
		// Return whether we have all information we need to start writing out data.
		// Since we don't really need any data in this use case, we are always ready.
		// A container might only be primed once it received at least one frame from each source
		// it will be muxing together (see Muxer.cpp for how this would work then).
		return true;
	}

	FILE* file;

};

int main()
{
	// This example will decode an audio stream from a container and output it as raw audio data in the chosen format.
	try
	{
		// Load this container file so we can extract audio from it.
		ffmpegcpp::Demuxer demuxer("samples/big_buck_bunny.mp4");

		// Create a file sink that will just output the raw audio data.
		auto fileSink = std::make_unique<RawAudioFileSink>("rawaudio");

		// tie the file sink to the best audio stream in the input container.
		demuxer.DecodeBestAudioStream(fileSink.get());

		// Prepare the output pipeline. This will push a small amount of frames to the file sink until it IsPrimed returns true.
		demuxer.PreparePipeline();

		// Push all the remaining frames through.
		while (!demuxer.IsDone())
		{
			demuxer.Step();
		}

	}
	catch (ffmpegcpp::FFmpegException e)
	{
		std::cerr << "Exception caught!" << '\n';
		std::cerr << e.what() << '\n';
		throw e;
	}

	std::cout << "Decoding complete!" << '\n';
	std::cout << "Press any key to continue..." << '\n';

	getchar();
}
