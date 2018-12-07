
#include <iostream>

#include "GeneratedVideoSource.h"
#include "GeneratedAudioSource.h"

#include <ffmpegcpp.h>

using namespace ffmpegcpp;
using namespace std;

// This example demonstrates different combinations and usages for ffmpeg-cpp.
// Configure it by changing the values below.
// The list of supported codecs is not limited to the ones mentioned below,
// this is just a list we support in this demo. You can use any codec that is
// supported by ffmpeg, but you might have to write your own Codec-class wrapper
// to make it work. Look at the examples in the Codecs-dir to see how it is done.
int main(int argc, char **argv)
{
	// These are example video and audio sources used below.
	const char* rawVideoFile = "samples/carphone_qcif.y4m";
	int rawVideoWidth = 176; int rawVideoHeight = 162;
	const char* rawAudioFile = "samples/Vivaldi_s16le_2_channels_samplerate_11025.dat";
	const char* rawAudioFormat = "s16le"; int rawAudioSampleRate = 11025; int rawAudioChannels = 2;

	const char* encodedVideoFile = "samples/carphone.h264";
	const char* encodedAudioFile = "samples/Vivaldi_Sonata_eminor_.mp3";

	const char* containerWithVideoAndAudioFile = "samples/big_buck_bunny.mp4";
	const char* containerWithAudioFile = "samples/DesiJourney.wav";

	// hard-code the settings here, but let them be overridden by the arguments
	string inputAudioSource = "GENERATED"; // options are RAW, ENCODED, CONTAINER, GENERATED
	string inputVideoSource = "ENCODED"; // options are RAW, ENCODED, CONTAINER, GENERATED
	string outputAudioCodec = "MP2"; // options are MP2, AAC, NONE
	string outputVideoCodec = "NONE"; // options are H264, H265, VP9, NONE
	string outputContainerName = "samples/out.mp4"; // container format is deduced from extension so use a known one

	// you can use any filter string that you can use in the ffmpeg command-line here
	// set the filter to NULL to disable filtering.
	// See https://trac.ffmpeg.org/wiki/FilteringGuide for more info
	// This example rotates the entire video and then puts a vignette on top of it.
	const char* videoFilterConfig = NULL;//"transpose=cclock[middle];[middle]vignette"

	// create the different components that make this come together
	try
	{

		/**
		 * CREATE THE OUTPUT CONTAINER
		*/

		// create the output muxer - we'll be adding encoders to it later
		Muxer* muxer = new Muxer(outputContainerName.c_str());


		/**
		 * CONFIGURE AUDIO OUTPUT
		*/

		// create the output encoder based on our setting above
		AudioCodec* audioCodec = nullptr;
		if (outputAudioCodec == "MP2")
		{
			audioCodec = new AudioCodec(AV_CODEC_ID_MP2);

		}
		else if (outputAudioCodec == "AAC")
		{
			audioCodec = new AudioCodec(AV_CODEC_ID_AAC);

		}
		else if (outputAudioCodec == "NONE")
		{
			// no codec specified - don't output audio!
		}

		// create an encoder - this encoder will receive raw data from any source (filter, file, container, memory, etc),
		// encode it and send it to the muxer (the output container).
		AudioEncoder* audioEncoder = nullptr;
		if (audioCodec != nullptr)
		{
			audioEncoder = new AudioEncoder(audioCodec, muxer);
		}


		/**
		 * CONFIGURE VIDEO OUTPUT
		*/

		// create the output encoder based on our setting above
		VideoCodec* videoCodec = nullptr;
		if (outputVideoCodec == "H264")
		{
			H264NVEncCodec* h264Codec = new H264NVEncCodec();
			h264Codec->SetPreset("hq");
			videoCodec = h264Codec;
		}
		else if (outputVideoCodec == "H265")
		{
			H265NVEncCodec* h265Codec = new H265NVEncCodec();
			h265Codec->SetPreset("hq");
			videoCodec = h265Codec;
		}
		else if (outputVideoCodec == "VP9")
		{
			VP9Codec* vp9Codec = new VP9Codec();
			vp9Codec->SetLossless(true);
			videoCodec = vp9Codec;
		}
		else if (outputVideoCodec == "NONE")
		{
			// no codec specified - don't output audio!
		}

		// create an encoder for the codec and tie it to the muxer
		// this encoder will receive data from an input source (file, raw, filter, etc), encode it and send it to the output container (muxer)
		VideoEncoder* videoEncoder = nullptr;
		if (videoCodec != nullptr)
		{
			videoEncoder = new VideoEncoder(videoCodec, muxer);
		}


		/**
		 * CONFIGURE AUDIO INPUT
		*/

		// only do this when there is an output - otherwise there is no point in reading audio
		InputSource* audioInputSource = nullptr;
		if (audioEncoder != nullptr)
		{
			if (inputAudioSource == "RAW")
			{
				audioInputSource = new RawAudioFileSource(rawAudioFile, rawAudioFormat, rawAudioSampleRate, rawAudioChannels, audioEncoder);
			}
			else if (inputAudioSource == "ENCODED")
			{
				audioInputSource = new EncodedFileSource(encodedAudioFile, AV_CODEC_ID_MP3, audioEncoder);
			}
			else if (inputAudioSource == "CONTAINER")
			{
				// if the input comes from a container, we use the demuxer class - it is just an input source like any other
				Demuxer* demuxer = new Demuxer(containerWithAudioFile);
				demuxer->EncodeBestAudioStream(audioEncoder);
				audioInputSource = demuxer;
			}
			else if (inputAudioSource == "GENERATED")
			{
				audioInputSource = new GeneratedAudioSource(audioEncoder);
			}
		}


		/**
		 * CONFIGURE VIDEO FILTER IF IT IS USED
		*/

		VideoFrameSink* videoFrameSink = videoEncoder;

		// If a video filter was specified, we inject it into the pipeline here.
		// Instead of feeding the video source directly to the encoder, we feed it to
		// the video filter instead, which will pass it on to the encoder.
		VideoFilter* videoFilter = nullptr;
		if (videoFilterConfig != NULL && videoEncoder != nullptr)
		{
			videoFilter = new VideoFilter(videoFilterConfig, videoEncoder);
			videoFrameSink = videoFilter; // used to feed the source below
		}
		// 


		/**
		 * CONFIGURE VIDEO INPUT
		*/

		// only do this when there is video output
		InputSource* videoInputSource = nullptr;
		if (videoEncoder != nullptr)
		{
			if (inputVideoSource == "RAW")
			{
				videoInputSource = new RawVideoFileSource(rawVideoFile, videoFrameSink);
			}
			else if (inputVideoSource == "ENCODED")
			{
				videoInputSource = new RawVideoFileSource(encodedVideoFile, videoFrameSink);
			}
			else if (inputVideoSource == "CONTAINER")
			{
				Demuxer* demuxer = new Demuxer(containerWithVideoAndAudioFile);
				demuxer->EncodeBestVideoStream(videoFrameSink);
				videoInputSource = demuxer;
			}
			else if (inputVideoSource == "GENERATED")
			{
				videoInputSource = new GeneratedVideoSource(640, 480, videoFrameSink);
			}
		}

		/**
		 * PROCESS THE DATA
		*/

		// finally, we can start writing data to our pipelines. Open the floodgates with Start()
		// to start reading frames from the input, decoding them, optionally filtering them,
		// encoding them and writing them to the final container.
		if (audioInputSource != nullptr) audioInputSource->Start();
		if (videoInputSource != nullptr) videoInputSource->Start();

		// Note: if you use a RawVideoDataSource directly, you can do this over time. You don't need
		// to do it all at once. This can be useful if you want to encode frames that come from rendering.

		// close the muxer and save the file to disk
		muxer->Close();


		// all done
		if (audioCodec != nullptr)
		{
			delete audioCodec;
			delete audioEncoder;
		}
		if (videoCodec != nullptr)
		{
			delete videoCodec;
			delete videoEncoder;
			if (videoFilter != nullptr) delete videoFilter;
		}

		if (audioInputSource != nullptr)
		{
			delete audioInputSource;
		}

		if (videoInputSource != nullptr)
		{
			printf("Delete video input...\n");
			delete videoInputSource;
		}

		delete muxer;
		printf("ALL DONE\n");
	}
	catch (FFmpegException e)
	{
		cerr << e.what() << endl;
		throw e;
	}

	cout << "Press any key to continue..." << endl;

	getchar();

	return 0;
}
