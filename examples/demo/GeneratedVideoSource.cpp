#include "GeneratedVideoSource.h"

GeneratedVideoSource::GeneratedVideoSource(int width, int height, FrameSink* frameSink)
{
	// generate a raw video source that will convert the raw format to any other format and pass it on to the encoder
	// or any other sink (might be a filter as well).
	output = new RawVideoDataSource(width, height, AV_PIX_FMT_RGBA, AV_PIX_FMT_RGBA, 30, frameSink);
}

GeneratedVideoSource::~GeneratedVideoSource()
{
	delete output;
	delete rgb;
}

uint8_t* generate_rgb(int width, int height, int pts, uint8_t *rgb);

void GeneratedVideoSource::PreparePipeline()
{
	while (!output->IsPrimed() && !IsDone())
	{
		Step();
	}
}

void GeneratedVideoSource::Step()
{
	rgb = generate_rgb(output->GetWidth(), output->GetHeight(), frameNumber, rgb);
	output->WriteFrame(rgb, 4 * output->GetWidth());
	++frameNumber;

	if (IsDone())
	{
		output->Close();
	}
}

bool GeneratedVideoSource::IsDone()
{
	return frameNumber >= 100;
}

uint8_t* generate_rgb(int width, int height, int pts, uint8_t *rgb)
{
	int x, y, cur;
	rgb = (uint8_t*)realloc(rgb, 4 * sizeof(uint8_t) * height * width);
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			cur = 4 * (y * width + x);
			rgb[cur + 0] = 0;
			rgb[cur + 1] = 0;
			rgb[cur + 2] = 0;
			rgb[cur + 3] = (pts % 25) * (255 / 25);
			if ((pts / 25) % 2 == 0)
			{
				if (y < height / 2)
				{
					if (x < width / 2)
					{
						/* Black. */
					}
					else
					{
						rgb[cur + 0] = 255;
					}
				}
				else
				{
					if (x < width / 2)
					{
						rgb[cur + 1] = 255;
					}
					else
					{
						rgb[cur + 2] = 255;
					}
				}
			}
			else
			{
				if (y < height / 2)
				{
					rgb[cur + 0] = 255;
					if (x < width / 2)
					{
						rgb[cur + 1] = 255;
					}
					else
					{
						rgb[cur + 2] = 255;
					}
				}
				else
				{
					if (x < width / 2)
					{
						rgb[cur + 1] = 255;
						rgb[cur + 2] = 255;
					}
					else
					{
						rgb[cur + 0] = 255;
						rgb[cur + 1] = 255;
						rgb[cur + 2] = 255;
					}
				}
			}
		}
	}
	return rgb;
}

