#pragma once

namespace Quark {

	enum class AudioFormat
	{
		None = 0,
		Integer8BitMono,
		Integer16BitMono,
		Integer8BitStereo,
		Integer16BitStereo,

		Float32BitMono,
		Float32BitStereo
	};

	constexpr AudioFormat GetFormat(uint16_t channels, uint16_t bitdepth, bool floatingPoint)
	{
		switch (channels)
		{
			case 1:
			{
				switch (bitdepth)
				{
					case 8:  return AudioFormat::Integer8BitMono;
					case 16: return AudioFormat::Integer16BitMono;

					QK_ASSERT_NO_DEFAULT("Invalid bitdepth");
				}

				break;
			}
			case 2:
			{
				switch (bitdepth)
				{
					case 8:  return AudioFormat::Integer8BitStereo;
					case 16: return AudioFormat::Integer16BitStereo;

					QK_ASSERT_NO_DEFAULT("Invalid bitdepth");
				}

				break;
			}

			QK_ASSERT_NO_DEFAULT("Unknown audio format");
		}

		return AudioFormat::None;
	}

	constexpr uint32_t GetChannelsCount(AudioFormat format)
	{
		switch (format)
		{
			case AudioFormat::Integer8BitMono:
			case AudioFormat::Integer16BitMono:
			case AudioFormat::Float32BitMono:
				return 1;
			case AudioFormat::Integer8BitStereo:
			case AudioFormat::Integer16BitStereo:
			case AudioFormat::Float32BitStereo:
				return 2;

			QK_ASSERT_NO_DEFAULT("Invalid channels format");
		}

		return 0;
	}
}
