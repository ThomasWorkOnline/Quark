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
		BitDepthFormat BitDepth{};
		ChannelsFormat Channels{};
	};

	static constexpr AudioFormat GetFormat(uint16_t channels, uint16_t bitdepth, bool floatingPoint)
	{
		AudioFormat format{};
		switch (channels)
		{
			case 1: format.Channels = ChannelsFormat::Mono;
				break;
			case 2: format.Channels = ChannelsFormat::Stereo;
				break;
			default:
				QK_CORE_FATAL("Audio format has invalid channels specification");
				break;
		}

		switch (bitdepth)
		{
			case 8:
				QK_CORE_ASSERT(!floatingPoint, "Invalid audio format: 8-bit floating-point is invalid");
				format.BitDepth = BitDepthFormat::Int8;
				break;
			case 16:
				QK_CORE_ASSERT(!floatingPoint, "Invalid audio format: 8-bit floating-point is invalid");
				format.BitDepth = BitDepthFormat::Int16;
				break;
			case 32:
				format.BitDepth = floatingPoint ? BitDepthFormat::Float32 : BitDepthFormat::Int32;
				break;
			default:
				QK_CORE_FATAL("Unknown audio bitdepth specification");
				break;
		}

		return format;
	}

	constexpr uint32_t GetChannelsCount(ChannelsFormat format)
	{
		switch (format)
		{
			case ChannelsFormat::Mono:   return 1;
			case ChannelsFormat::Stereo: return 2;
			default:
				QK_CORE_FATAL("Invalid channels format");
				return 0;
		}
	}
}
