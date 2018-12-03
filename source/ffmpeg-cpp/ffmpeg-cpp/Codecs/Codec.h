#pragma once

#include "ffmpeg.h"
#include "std.h"

#include "OpenCodec.h"

namespace ffmpegcpp
{

	class Codec
	{
	public:

		Codec(const char* codecName);
		Codec(AVCodecID codecId);
		~Codec();

		void SetOption(const char* name, const char* value);
		void SetOption(const char* name, int value);
		void SetOption(const char* name, double value);

		OpenCodec* Open();

	protected:

		AVCodecContext* codecContext = nullptr;

	private:

		void CleanUp();

		AVCodecContext* LoadContext(AVCodec* codec);

		bool opened = false;
	};
}
