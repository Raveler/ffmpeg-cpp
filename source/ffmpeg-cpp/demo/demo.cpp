
#include <iostream>

#include "GeneratedVideoSource.h"

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
	// these are example video and audio sources used below
	const char* rawVideoFile = "samples/carphone_qcif.y4m";
	int rawVideoWidth = 176; int rawVideoHeight = 162;
	const char* rawAudioFile = "samples/Vivaldi_s16le_2_channels_samplerate_11025.dat";
	const char* rawAudioFormat = "s16le"; int rawAudioSampleRate = 11025;

	const char* encodedVideoFile = "samples/carphone.h264";
	int encodedVideoWidth = 176; int encodedVideoHeight = 162;
	const char* encodedAudioFile = "samples/Vivaldi_Sonata_eminor_.mp3";

	const char* containerWithVideoAndAudioFile = "samples/big_buck_bunny.mp4";
	int containerVideoWidth = 640; int containerVideoHeight = 386;
	const char* containerWithAudioFile = "samples/DesiJourney.wav";

	// hard-code the settings here, but let them be overridden by the arguments
	string inputAudioSource = "RAW"; // options are RAW, ENCODED, CONTAINER
	string inputVideoSource = "GENERATED"; // options are RAW, ENCODED, CONTAINER, GENERATED
	string outputAudioCodec = "MP2"; // options are MP2, AAC, NONE
	string outputVideoCodec = "H264"; // options are H264, H265, VP9, NONE
	string outputContainerName = "out.mp4"; // container format is deduced from extension so use a known one

	// you can use any filter string that you can use in the ffmpeg command-line here
	// set the filter to NULL to disable filtering.
	const char* videoFilterConfig = "vignette";

	// create the different components that make this come together
	try
	{
		// CONFIGURE AUDIO OUTPUT

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

		// if a codec was specified, we process it now
		AudioOutputStream* audioStream = nullptr;
		AudioEncoder* audioEncoder = nullptr;
		OpenCodec* openAudioCodec = nullptr;
		if (audioCodec != nullptr)
		{
			// open the codec so we can use it in the system
			openAudioCodec = audioCodec->Open();

			// create an audio stream from the codec that we will mux into a container later
			audioStream = new AudioOutputStream(openAudioCodec);

			// create an encoder for the stream - this encoder will receive raw data from any source (filter, file, container, memory, etc)
			audioEncoder = new AudioEncoder(audioStream);
		}

		// CONFIGURE VIDEO OUTPUT

		// first properly set the video resolution based on the input file - we don't want to do any rescaling
		// (even though we could if we activated the filter!).
		int videoWidth;
		int videoHeight;
		if (inputVideoSource == "RAW")
		{
			videoWidth = rawVideoWidth; videoHeight = rawVideoHeight;
		}
		else if (inputVideoSource == "ENCODED")
		{
			videoWidth = encodedVideoWidth; videoHeight = encodedVideoHeight;
		}
		else if (inputVideoSource == "CONTAINER")
		{
			videoWidth = containerVideoWidth; videoHeight = containerVideoHeight;
		}
		else if (inputVideoSource == "GENERATED")
		{
			videoWidth = containerVideoWidth; videoHeight = containerVideoHeight;
		}

		// create the output encoder based on our setting above
		VideoCodec* videoCodec = nullptr;
		if (outputVideoCodec == "H264")
		{
			H264NVEncCodec* h264Codec = new H264NVEncCodec(videoWidth, videoHeight, 30, AV_PIX_FMT_YUV420P);
			h264Codec->SetPreset("hq");
			videoCodec = h264Codec;
		}
		else if (outputVideoCodec == "H265")
		{
			H265NVEncCodec* h265Codec = new H265NVEncCodec(videoWidth, videoHeight, 30, AV_PIX_FMT_YUV420P);
			h265Codec->SetPreset("hq");
			videoCodec = h265Codec;
		}
		else if (outputVideoCodec == "VP9")
		{
			VP9Codec* vp9Codec = new VP9Codec(videoWidth, videoHeight, 30, AV_PIX_FMT_YUV420P);
			vp9Codec->SetLossless(true);
			videoCodec = vp9Codec;
		}
		else if (outputVideoCodec == "NONE")
		{
			// no codec specified - don't output audio!
		}

		// if a codec was specified, we process it now
		VideoOutputStream* videoStream = nullptr;
		VideoEncoder* videoEncoder = nullptr;
		OpenCodec* openVideoCodec = nullptr;
		if (videoCodec != nullptr)
		{
			// open the codec so we can use it in the system
			openVideoCodec = videoCodec->Open();

			// create an audio stream from the codec that we will mux into a container later
			videoStream = new VideoOutputStream(openVideoCodec);

			// create an encoder for the stream - this encoder will receive raw data from any source (filter, file, container, memory, etc)
			videoEncoder = new VideoEncoder(videoStream);
		}


		// CONFIGURE AUDIO INPUT

		// only do this when there is an output - otherwise there is no point in reading audio
		InputSource* audioInputSource = nullptr;
		if (audioEncoder != nullptr)
		{
			if (inputAudioSource == "RAW")
			{
				// TODO RENAME TO ENCODED AND WRITE AN ACTUAL RAW FILE SOURCE
				audioInputSource = new RawAudioFileSource(rawAudioFile, rawAudioFormat, rawAudioSampleRate, audioEncoder);
			}
			else if (inputAudioSource == "ENCODED")
			{
				audioInputSource = new EncodedFileSource(encodedAudioFile, AV_CODEC_ID_MP3, audioEncoder);
			}
			else if (inputAudioSource == "CONTAINER")
			{
				Demuxer* demuxer = new Demuxer(containerWithAudioFile);
				demuxer->EncodeBestAudioStream(audioEncoder);
				audioInputSource = demuxer;
			}
		}

		// CONFIGURE VIDEO FILTER IF IT IS USED
		VideoFrameSink* videoFrameSink = videoEncoder;

		// If a video filter was specified, we inject it into the pipeline here.
		// Instead of feeding the video source directly to the encoder, we feed it to
		// the video filter instead, which will pass it on to the encoder.
		VideoFilter* videoFilter;
		if (videoFilterConfig != NULL && videoEncoder != nullptr)
		{
			videoFilter = new VideoFilter(videoFilterConfig, videoEncoder);
			videoFrameSink = videoFilter; // used to feed the source below
		}
		// 

		// CONFIGURE VIDEO INPUT

		// only do this when there is video output
		InputSource* videoInputSource = nullptr;
		if (videoEncoder != nullptr)
		{
			if (inputVideoSource == "RAW")
			{
				// TODO RENAME TO ENCODED AND WRITE AN ACTUAL RAW FILE SOURCE
				//RawFileSource* audioSource = new RawFileSource(rawAudioFile, AV_CODEC_ID_MP3, audioEncoder);
			}
			else if (inputVideoSource == "ENCODED")
			{
				videoInputSource = new EncodedFileSource(encodedVideoFile, AV_CODEC_ID_H264, videoFrameSink);
			}
			else if (inputVideoSource == "CONTAINER")
			{
				Demuxer* demuxer = new Demuxer(containerWithVideoAndAudioFile);
				demuxer->EncodeBestVideoStream(videoFrameSink);
				videoInputSource = demuxer;
			}
			else if (inputVideoSource == "GENERATED")
			{
				videoInputSource = new GeneratedVideoSource(videoWidth, videoHeight, videoFrameSink);
			}
		}


		// CREATE THE MUXER - THE OUTPUT CONTAINER
		vector<OutputStream*> outputStreams;
		if (audioStream != nullptr) outputStreams.push_back(audioStream);
		if (videoStream != nullptr) outputStreams.push_back(videoStream);

		// create the output muxer
		Muxer* muxer = new Muxer(outputContainerName.c_str(), outputStreams);

		// activate both the audio and video sources IF they are available
		if (audioInputSource != nullptr) audioInputSource->Start();
		if (videoInputSource != nullptr) videoInputSource->Start();

		// close the muxer and save the file to disk
		muxer->Close();


		// all done
		if (audioCodec != nullptr)
		{
			delete audioCodec;
			delete openAudioCodec;
			delete audioStream;
			delete audioEncoder;
		}
		if (videoCodec != nullptr)
		{
			delete videoCodec;
			delete openVideoCodec;
			delete videoStream;
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

	cout << "Press any key to continue..." << endl;

	getchar();

	return 0;
}
