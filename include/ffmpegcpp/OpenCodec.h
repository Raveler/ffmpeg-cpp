#pragma once

struct AVCodecContext;

namespace ffmpegcpp
{
	class OpenCodec
	{
	public:

		OpenCodec(AVCodecContext* openCodecContext);
		~OpenCodec();

		AVCodecContext* GetContext() const;

	private:

		AVCodecContext* context;
	};


}
