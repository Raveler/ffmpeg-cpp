
#include <iostream>

#include "ffmpegcpp.h"

using namespace std;
using namespace ffmpegcpp;

class RawAudioFileSink : public AudioFrameSink, public FrameWriter
{
public:

	RawAudioFileSink(const char* fileName)
	{
		file = fopen(fileName, "wb");
	}

	FrameSinkStream* CreateStream()
	{
		stream = new FrameSinkStream(this, 0);
		return stream;
	}

	virtual void WriteFrame(int streamIndex, AVFrame* frame, StreamData* streamData)
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

	virtual void Close(int streamIndex)
	{
		fclose(file);
		delete stream;
	}

	virtual bool IsPrimed()
	{
		// Return whether we have all information we need to start writing out data.
		// Since we don't really need any data in this use case, we are always ready.
		// A container might only be primed once it received at least one frame from each source
		// it will be muxing together (see Muxer.cpp for how this would work then).
		return true;
	}

private:
	FILE* file;
	FrameSinkStream* stream;

};

int main()
{
	// This example will decode an audio stream from a container and output it as raw audio data in the chosen format.
	try
	{
		// Load this container file so we can extract audio from it.
		Demuxer* demuxer = new Demuxer("samples/big_buck_bunny.mp4");

		// Create a file sink that will just output the raw audio data.
		RawAudioFileSink* fileSink = new RawAudioFileSink("rawaudio");

		// tie the file sink to the best audio stream in the input container.
		demuxer->DecodeBestAudioStream(fileSink);

		// Prepare the output pipeline. This will push a small amount of frames to the file sink until it IsPrimed returns true.
		demuxer->PreparePipeline();

		// Push all the remaining frames through.
		while (!demuxer->IsDone())
		{
			demuxer->Step();
		}

		// done
		delete demuxer;
		delete fileSink;
	}
	catch (FFmpegException e)
	{
		cerr << "Exception caught!" << endl;
		cerr << e.what() << endl;
		throw e;
	}

	cout << "Decoding complete!" << endl;
	cout << "Press any key to continue..." << endl;

	getchar();
}
