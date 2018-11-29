
#include <iostream>

#include <ffmpegcpp.h>

using namespace ffmpegcpp;

int main(int argc, char **argv)
{
	char* inFilename = argv[1];
	char* outFilename = argv[2];

	// create the output stream
	try
	{
		H264NVEncCodec* codec = new H264NVEncCodec(177, 144, 30, AV_PIX_FMT_YUV420P);
		codec->SetPreset("hq");
		OpenCodec* openCodec = codec->Open();
		OutputStream* stream = new OutputStream(openCodec);

		// list of output streams for the muxer
		vector<OutputStream*> streams;
		streams.push_back(stream);

		// create the encoder that will link the source and muxer together
		Encoder* encoder = new Encoder(stream);

		// create an input source

		// MP3: AV_CODEC_ID_MP3
		// WAV: AV_CODEC_ID_FIRST_AUDIO
		// VIDEO: AV_CODEC_ID_RAWVIDEO
		RawFileSource* source = new RawFileSource(inFilename, AV_CODEC_ID_H264, encoder);

		// create the output muxer
		Muxer* muxer = new Muxer(outFilename, streams);

		source->Start();

		muxer->Close();


		// all done
		delete muxer;
		delete source;
		delete encoder;
		delete stream;
		delete openCodec;
		delete codec;
	}
	catch (FFmpegException e)
	{
		cerr << e.what() << endl;
		throw e;
	}
	catch (...)
	{
		cout << "OMG! an unexpected exception has been caught" << endl;
	}

	cout << "Press any key to continue..." << endl;

	getchar();

	return 0;
}
