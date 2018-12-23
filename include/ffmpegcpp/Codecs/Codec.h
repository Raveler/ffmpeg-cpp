#pragma once

#include "FFmpegResource.h"

#include <memory>
#include <string>

enum AVCodecID;
struct AVCodecContext;

namespace ffmpegcpp
{
	class OpenCodec;

	class Codec
	{
	public:

		Codec(const std::string & codecName);
		Codec(AVCodecID codecId);

		void SetOption(const std::string & name, const std::string & value);
		void SetOption(const std::string & name, int value);
		void SetOption(const std::string & name, double value);

		void SetGlobalContainerHeader(); // used by the Muxer for configuration purposes

	protected:

		FFmpegResource<AVCodecContext> codecContext;

		std::unique_ptr<OpenCodec> Open();

	private:

		bool opened = false;
	};
}
