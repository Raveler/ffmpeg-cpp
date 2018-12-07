#include "GeneratedAudioSource.h"

GeneratedAudioSource::GeneratedAudioSource(AudioFrameSink* frameSink)
{
	this->sampleRate = 44100;
	this->channels = 2;
	this->format = AV_SAMPLE_FMT_S16;

	// generate a raw video source that will convert the raw format to any other format and pass it on to the encoder
	// or any other sink (might be a filter as well).
	output = new RawAudioDataSource(format, this->sampleRate, this->channels, frameSink);

}

GeneratedAudioSource::~GeneratedAudioSource()
{
	delete output;
}

uint8_t* generate_rgb(int width, int height, int pts, uint8_t *rgb);

void GeneratedAudioSource::Start()
{
	// 2 channels, 2 bytes per channel (16 bits)
	int sampleCount = 512;
	uint16_t* samples = new uint16_t[channels * 2 * sampleCount];

	/* encode a single tone sound */
	float t = 0.0f;
	float tincr = 2 * M_PI * 440.0 / sampleRate;
	for (int i = 0; i < 200; i++)
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
	}

	output->Close();

	delete samples;
}
