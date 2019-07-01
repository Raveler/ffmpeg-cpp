
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
void PlayDemo(int argc, char** argv)
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
	string inputAudioSource = "CONTAINER"; // options are RAW, ENCODED, CONTAINER, GENERATED
	string inputVideoSource = "ENCODED"; // options are RAW, ENCODED, CONTAINER, GENERATED
	string outputAudioCodec = "NONE"; // options are MP2, AAC, NONE
	string outputVideoCodec = "H264"; // options are H264, H265, VP9, NONE (H264 and H265 only work on Nvidia hardware)
	string outputContainerName = "out.mp4"; // container format is deduced from extension so use a known one

	// you can use any filter string that you can use in the ffmpeg command-line here
	// set the filter to NULL to disable filtering.
	// See https://trac.ffmpeg.org/wiki/FilteringGuide for more info
	// This example rotates the entire video and then puts a vignette on top of it.
	const char* videoFilterConfig = "transpose=cclock[middle];[middle]vignette";
	//const char* videoFilterConfig = NULL;

	// if command line is specified, we overwrite our hard-coded settings
	if (argc >= 6)
	{
		inputAudioSource = string(argv[1]);
		inputVideoSource = string(argv[2]);
		outputAudioCodec = string(argv[3]);
		outputVideoCodec = string(argv[4]);
		videoFilterConfig = argv[5];
	}


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
			printf("Encoding audio as MP2...\n");
			audioCodec = new AudioCodec(AV_CODEC_ID_MP2);

		}
		else if (outputAudioCodec == "AAC")
		{
			printf("Encoding audio as AAC...\n");
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
			printf("Encoding video as H264 on Nvidia GPU...\n");
			H264NVEncCodec* h264Codec = new H264NVEncCodec();
			h264Codec->SetPreset("hq");
			videoCodec = h264Codec;
		}
		else if (outputVideoCodec == "H265")
		{
			printf("Encoding video as H265 on Nvidia GPU...\n");
			H265NVEncCodec* h265Codec = new H265NVEncCodec();
			h265Codec->SetPreset("hq");
			videoCodec = h265Codec;
		}
		else if (outputVideoCodec == "VP9")
		{
			printf("Encoding video as VP9...\n");
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
				printf("Pulling audio from %s...\n", rawAudioFile);
				audioInputSource = new RawAudioFileSource(rawAudioFile, rawAudioFormat, rawAudioSampleRate, rawAudioChannels, audioEncoder);
			}
			else if (inputAudioSource == "ENCODED")
			{
				printf("Pulling audio from %s...\n", encodedAudioFile);
				audioInputSource = new EncodedFileSource(encodedAudioFile, AV_CODEC_ID_MP3, audioEncoder);
			}
			else if (inputAudioSource == "CONTAINER")
			{
				// if the input comes from a container, we use the demuxer class - it is just an input source like any other
				printf("Pulling audio from %s...\n", containerWithAudioFile);
				Demuxer* demuxer = new Demuxer(containerWithAudioFile);
				demuxer->DecodeBestAudioStream(audioEncoder);
				audioInputSource = demuxer;
			}
			else if (inputAudioSource == "GENERATED")
			{
				printf("Generating 440Hz audio tone...\n");
				audioInputSource = new GeneratedAudioSource(audioEncoder);
			}
		}

		/**
			* CONFIGURE VIDEO FILTER IF IT IS USED
		*/

		FrameSink* videoFrameSink = videoEncoder;

		// If a video filter was specified, we inject it into the pipeline here.
		// Instead of feeding the video source directly to the encoder, we feed it to
		// the video filter instead, which will pass it on to the encoder.
		Filter* videoFilter = nullptr;
		if (videoFilterConfig != NULL && videoEncoder != nullptr)
		{
			printf("Applying filter %s to video...\n", videoFilterConfig);
			videoFilter = new Filter(videoFilterConfig, videoEncoder);
			videoFrameSink = videoFilter; // used to feed the source below
		}

		/**
			* CONFIGURE VIDEO INPUT
		*/

		// only do this when there is video output
		InputSource* videoInputSource = nullptr;
		if (videoEncoder != nullptr)
		{
			if (inputVideoSource == "RAW")
			{
				printf("Pulling video from %s...\n", rawVideoFile);
				videoInputSource = new RawVideoFileSource(rawVideoFile, videoFrameSink);
			}
			else if (inputVideoSource == "ENCODED")
			{
				printf("Pulling video from %s...\n", encodedVideoFile);
				videoInputSource = new RawVideoFileSource(encodedVideoFile, videoFrameSink);
			}
			else if (inputVideoSource == "CONTAINER")
			{
				printf("Pulling video from %s...\n", containerWithVideoAndAudioFile);
				Demuxer* demuxer = new Demuxer(containerWithVideoAndAudioFile);
				demuxer->DecodeBestVideoStream(videoFrameSink);
				videoInputSource = demuxer;
			}
			else if (inputVideoSource == "GENERATED")
			{
				printf("Generating checkerboard video pattern...\n");
				videoInputSource = new GeneratedVideoSource(640, 480, videoFrameSink);
			}
		}


		/**
		* PROCESS THE DATA
		*/


		// As an awkward but necessary first step, we need to prime each input source.
		// This step decodes a part of the input data and from this, generates info
		// about the stream and propagates it all the way to the output container,
		// which needs to write this data to its header.
		if (videoInputSource != nullptr) videoInputSource->PreparePipeline();
		if (audioInputSource != nullptr) audioInputSource->PreparePipeline();



		// finally, we can start writing data to our pipelines. Open the floodgates
		// to start reading frames from the input, decoding them, optionally filtering them,
		// encoding them and writing them to the final container.
		// This can be interweaved if you want to.
		if (audioInputSource != nullptr)
		{
			while (!audioInputSource->IsDone()) audioInputSource->Step();
		}
		if (videoInputSource != nullptr)
		{
			while (!videoInputSource->IsDone()) videoInputSource->Step();
		}

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
			delete videoInputSource;
		}


		delete muxer;
	}
	catch (FFmpegException e)
	{
		cerr << e.what() << endl;
		throw e;
	}
}

int main(int argc, char **argv)
{
	PlayDemo(argc, argv);

	cout << "Encoding complete!" << endl;
	cout << "Press any key to continue..." << endl;

	getchar();

	return 0;
}