#include "RawAudioDataSource.h"
#include "FFmpegException.h"

namespace ffmpegcpp
{

	RawAudioDataSource::RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, int channels, AudioFrameSink* output)
		: RawAudioDataSource(sampleFormat, sampleRate, channels, av_get_default_channel_layout(channels), output)
	{
	}

	RawAudioDataSource::RawAudioDataSource(AVSampleFormat sampleFormat, int sampleRate, int channels, int64_t channelLayout, AudioFrameSink* output)
	{
		this->output = output->CreateStream();

		// create the frame
		int ret;

		frame = av_frame_alloc();
		if (!frame)
		{
			CleanUp();
			throw FFmpegException("Could not allocate video frame");
		}

		frame->format = sampleFormat;
		frame->sample_rate = sampleRate;
		frame->channels = channels;
		frame->channel_layout = channelLayout;
		frame->nb_samples = 735;

		// allocate the buffers for the frame data
		ret = av_frame_get_buffer(frame, 0);
		if (ret < 0)
		{
			CleanUp();
			throw FFmpegException("Could not allocate the video frame data", ret);
		}

	}

	RawAudioDataSource::~RawAudioDataSource()
	{
		CleanUp();
	}

	void RawAudioDataSource::CleanUp()
	{
		if (frame != nullptr)
		{
			av_frame_free(&frame);
			frame = nullptr;
		}
	}

	void RawAudioDataSource::WriteData(void* data, int sampleCount)
	{
		// resize the frame to the input
		frame->nb_samples = sampleCount;

		int ret = av_frame_make_writable(frame);
		if (ret < 0)
		{
			throw FFmpegException("Failed to make audio frame writable", ret);
		}

		// copy the data to the frame buffer
		int bytesPerSample = av_get_bytes_per_sample((AVSampleFormat)frame->format);
		memcpy(*frame->data, data, frame->nb_samples * frame->channels * bytesPerSample);

		// pass on to the sink
		// we don't have a time_base so we pass NULL and hope that it gets handled later...
		output->WriteFrame(frame, NULL);
	}

	void RawAudioDataSource::Close()
	{
		output->Close();
	}

	bool RawAudioDataSource::IsPrimed()
	{
		return output->IsPrimed();
	}
}
