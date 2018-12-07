#pragma once

namespace ffmpegcpp
{
	class InputSource
	{
	public:

		virtual ~InputSource() {}

		virtual void Start() = 0;
	};
}


