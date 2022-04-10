#pragma once

namespace Quark {

	enum class BitDepthFormat
	{
		None = 0,
		Int8,
		Int16,
		Int32,
		Float32
	};

	enum class ChannelsFormat
	{
		None = 0,
		Mono,
		Stereo
	};
	
	struct AudioFormat
	{
		BitDepthFormat BitDepth = BitDepthFormat::Int16;
		ChannelsFormat Channels = ChannelsFormat::Mono;
	};
}
