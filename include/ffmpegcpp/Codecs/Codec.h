#pragma once

#include "ffmpeg.h"

#include "OpenCodec.h"
#include "FFmpegResource.h"

#include <memory>

namespace ffmpegcpp
{

	class Codec
	{
	public:

		Codec(const char* codecName);
		Codec(AVCodecID codecId);

		void SetOption(const char* name, const char* value);
		void SetOption(const char* name, int value);
		void SetOption(const char* name, double value);

		void SetGlobalContainerHeader(); // used by the Muxer for configuration purposes

	protected:

		FFmpegResource<AVCodecContext> codecContext;

		std::unique_ptr<OpenCodec> Open();

	private:

		bool opened = false;
	};
}
