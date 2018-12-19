#pragma once

namespace ffmpegcpp
{
	class InputSource
	{
	public:

		virtual ~InputSource() {}

		virtual void PreparePipeline() = 0;
		virtual bool IsDone() const = 0;
		virtual void Step() = 0;

	};
}


