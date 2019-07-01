#include "GeneratedAudioSource.h"

GeneratedAudioSource::GeneratedAudioSource(FrameSink* frameSink)
{
	this->sampleRate = 44100;
	this->channels = 2;
	this->format = AV_SAMPLE_FMT_S16;

	// generate a raw video source that will convert the raw format to any other format and pass it on to the encoder
	// or any other sink (might be a filter as well).
	output = new RawAudioDataSource(format, this->sampleRate, this->channels, frameSink);

	samples = new uint16_t[channels * 2 * sampleCount];

}

GeneratedAudioSource::~GeneratedAudioSource()
{
	delete output;
	delete samples;
}

void GeneratedAudioSource::PreparePipeline()
{
	while (!output->IsPrimed() && !IsDone())
	{
		Step();
	}
}

bool GeneratedAudioSource::IsDone()
{
	return frameNumber >= 120;
}

void GeneratedAudioSource::Step()
{
	/* encode a single tone sound */
	float t = 0.0f;
	float tincr = 2 * M_PI * 440.0 / sampleRate;
	for (int i = 0; i < 120; i++)
	{
		/* make sure the frame is writable -- makes a copy if the encoder
		 * kept a reference internally */

		for (int j = 0; j < sampleCount; j++)
		{
			samples[2 * j] = (int)(sin(t) * 10000);

			for (int k = 1; k < channels; k++)
				samples[2 * j + k] = samples[2 * j];
			t += tincr;
		}

		// submit to the sink
		output->WriteData(samples, sampleCount);
		++frameNumber;
	}

	if (IsDone())
	{
		output->Close();
	}
}
