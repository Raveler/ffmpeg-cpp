#pragma once

struct AVCodecContext;

namespace ffmpegcpp
{
	class OpenCodec
	{
	public:

		OpenCodec(AVCodecContext* openCodecContext);

        AVCodecContext* GetContext() const;

	private:

		AVCodecContext* context;
	};


}
