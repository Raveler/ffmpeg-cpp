// AniHub.LipSync.cpp : Defines the exported functions for the DLL application.
//

#include "SimpleInterface.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "ffmpegcpp.h"
#include "ffmpeg.h"

using namespace std;
using namespace ffmpegcpp;


struct Context
{
	Muxer* muxer = nullptr;

	VideoCodec* videoCodec = nullptr;
	AudioCodec* audioCodec = nullptr;

	Demuxer* videoDemuxer = nullptr;
	Demuxer* audioDemuxer = nullptr;

	const char* videoFilterString = nullptr;
	const char* audioFilterString = nullptr;

	Filter* videoFilter = nullptr;
	Filter* audioFilter = nullptr;

	VideoFrameSink* videoEncoder = nullptr;
	AudioFrameSink* audioEncoder = nullptr;

	RawVideoDataSource* source = nullptr;

	vector<Demuxer*> uniqueDemuxers;

	bool errored = false;
	string error;
};

void SetError(Context* ctx, string error)
{
	ctx->errored = true;
	ctx->error = string(error);
}

void CleanUp(Context* ctx)
{
	if (ctx->muxer != nullptr) delete ctx->muxer;

	if (ctx->videoEncoder != nullptr) delete ctx->videoEncoder;
	if (ctx->audioEncoder != nullptr) delete ctx->audioEncoder;

	if (ctx->videoFilter != nullptr) delete ctx->videoFilter;
	if (ctx->audioFilter != nullptr) delete ctx->audioFilter;

	if (ctx->videoCodec != nullptr) delete ctx->videoCodec;
	if (ctx->audioCodec != nullptr) delete ctx->audioCodec;

	for (int i = 0; i < ctx->uniqueDemuxers.size(); ++i)
	{
		delete ctx->uniqueDemuxers[i];
	}

	delete ctx;
}

Demuxer* GetExistingDemuxer(Context* ctx, const char* fileName)
{
	if (ctx->videoDemuxer != nullptr && string(ctx->videoDemuxer->GetFileName()) == string(fileName)) return ctx->videoDemuxer;
	if (ctx->audioDemuxer != nullptr && string(ctx->audioDemuxer->GetFileName()) == string(fileName)) return ctx->audioDemuxer;
	return nullptr;
}

void* ffmpegCppCreate(const char* outputFileName)
{
	Context* ctx = new Context();
	try
	{
		// create the output muxer but don't add any streams yet
		ctx->muxer = new Muxer(outputFileName);
		return ctx;
	}
	catch (FFmpegException e)
	{
		SetError(ctx, string("Failed to create output file " + string(outputFileName) + ": " + string(e.what())));
		return nullptr;
	}
}

void ffmpegCppAddVideoStream(void* handle, const char* videoFileName)
{
	Context* ctx = (Context*)handle;
	try
	{
		// create the demuxer or re-use the previous one
		ctx->videoDemuxer = GetExistingDemuxer(ctx, videoFileName);
		if (ctx->videoDemuxer == nullptr)
		{
			ctx->videoDemuxer = new Demuxer(videoFileName);
			ctx->uniqueDemuxers.push_back(ctx->videoDemuxer);
		}

		// create the encoder
		ctx->videoCodec = new VideoCodec(ctx->muxer->GetDefaultVideoFormat()->id);
		ctx->videoEncoder = new VideoEncoder(ctx->videoCodec, ctx->muxer);
	}
	catch (FFmpegException e)
	{
		SetError(ctx, string("Failed to add video stream " + string(videoFileName) + ": " + string(e.what())));
	}
}

void ffmpegCppAddAudioStream(void* handle, const char* audioFileName)
{
	Context* ctx = (Context*)handle;
	try
	{
		// create the demuxer or re-use the previous one
		ctx->audioDemuxer = GetExistingDemuxer(ctx, audioFileName);
		if (ctx->audioDemuxer == nullptr)
		{
			ctx->audioDemuxer = new Demuxer(audioFileName);
			ctx->uniqueDemuxers.push_back(ctx->audioDemuxer);
		}

		// create the encoder
		ctx->audioCodec = new AudioCodec(ctx->muxer->GetDefaultAudioFormat()->id);
		ctx->audioEncoder = new AudioEncoder(ctx->audioCodec, ctx->muxer);
	}
	catch (FFmpegException e)
	{
		SetError(ctx, string("Failed to add audio stream " + string(audioFileName) + ": " + string(e.what())));
	}
}

void ffmpegCppAddVideoFilter(void* handle, const char* filterString)
{
	Context* ctx = (Context*)handle;
	ctx->videoFilterString = filterString;
}

void ffmpegCppAddAudioFilter(void* handle, const char* filterString)
{
	Context* ctx = (Context*)handle;
	ctx->audioFilterString = filterString;
}

void ffmpegCppGenerate(void* handle)
{
	Context* ctx = (Context*)handle;
	try
	{
		// create a filter if necessary
		FrameSink* videoFrameSink = ctx->videoEncoder;
		FrameSink* audioFrameSink = ctx->audioEncoder;
		if (ctx->videoFilterString != nullptr)
		{
			ctx->videoFilter = new Filter(ctx->videoFilterString, ctx->videoEncoder);
			videoFrameSink = ctx->videoFilter;
		}
		if (ctx->audioFilterString != nullptr)
		{
			ctx->audioFilter = new Filter(ctx->audioFilterString, ctx->audioEncoder);
			audioFrameSink = ctx->audioFilter;
		}

		// connect the input and output streams
		if (ctx->videoDemuxer != nullptr)
		{
			ctx->videoDemuxer->DecodeBestVideoStream(videoFrameSink);
		}
		if (ctx->audioDemuxer != nullptr)
		{
			ctx->audioDemuxer->DecodeBestAudioStream(audioFrameSink);
		}

		// now go over all the streams and process all frames
		for (int i = 0; i < ctx->uniqueDemuxers.size(); ++i)
		{
			ctx->uniqueDemuxers[i]->PreparePipeline();
		}

		// finally, we can start writing data to our pipelines. Open the floodgates
		// to start reading frames from the input, decoding them, optionally filtering them,
		// encoding them and writing them to the final container.
		// This can be interweaved if you want to.
		for (int i = 0; i < ctx->uniqueDemuxers.size(); ++i)
		{
			Demuxer* demuxer = ctx->uniqueDemuxers[i];
			while (!demuxer->IsDone()) demuxer->Step();
		}

		// close the muxer and save the file to disk
		ctx->muxer->Close();
	}
	catch (FFmpegException e)
	{
		SetError(ctx, string("Failed to generate output file: " + string(e.what())));
	}
}

void ffmpegCppAddFilter(void* handle, const char* filterString)
{
	// TODO
}


bool ffmpegCppIsError(void* handle)
{
	return ((Context*)handle)->errored;
}

const char* ffmpegCppGetError(void* handle)
{
	return ((Context*)handle)->error.c_str();
}

void ffmpegCppClose(void* handle)
{
	CleanUp((Context*)handle);
}