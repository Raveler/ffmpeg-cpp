
#include <iostream>

#include "ffmpegcpp.h"

using namespace std;
using namespace ffmpegcpp;

int main()
{
	// This example will print info about a video.
	try
	{

		// Load a video from a container and read its info.
		const char* fileName = "samples/big_buck_bunny.mp4";
		Demuxer* demuxer = new Demuxer(fileName);
		ContainerInfo info = demuxer->GetInfo();

		// Print the data similar to ffmpeg.exe.
		cout << "Input " << info.format->name << " from '" << fileName << "'" << endl;

		cout << "Video streams:" << endl;
		for (int i = 0; i < info.videoStreams.size(); ++i)
		{
			VideoStreamInfo stream = info.videoStreams[i];
			cout << "Stream #" << (i + 1)
				<< ": codec " << stream.codec->name
				<< ", pixel format " << stream.formatName
				<< ", resolution " << stream.width << "x" << stream.height
				<< ", bit rate " << stream.bitRate << "kb/s"
				<< ", fps " << ((float)stream.frameRate.num / (float)stream.frameRate.den)
				<< ", time base " << stream.timeBase.num  << "/" << stream.timeBase.den
				<< ", " << demuxer->GetFrameCount(stream.id) << " frames"
				<< endl;
		}

		cout << "Audio streams:" << endl;
		for (int i = 0; i < info.audioStreams.size(); ++i)
		{
			AudioStreamInfo stream = info.audioStreams[i];
			cout << "Stream #" << (i + 1)
				<< ": codec " << stream.codec->name
				<< ", channel layout " << stream.channelLayoutName
				<< ", channels " << stream.channels
				<< ", bit rate " << stream.bitRate << "kb/s"
				<< ", sample rate " << stream.sampleRate
				<< ", time base " << stream.timeBase.num << "/" << stream.timeBase.den
				<< ", " << demuxer->GetFrameCount(stream.id) << " frames"
				<< endl;
		}



	}
	catch (FFmpegException e)
	{
		cerr << "Exception caught!" << endl;
		cerr << e.what() << endl;
		throw e;
	}

	cout << "Encoding complete!" << endl;
	cout << "Press any key to continue..." << endl;

	getchar();
}
