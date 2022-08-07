#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Audio/AudioFormats.h"

#include <AL/al.h>

namespace Quark {

	constexpr ALenum AudioFormatToOpenALFormat(AudioFormat format)
	{
		switch (format)
		{
			case AudioFormat::Integer8BitMono:    return AL_FORMAT_MONO8;
			case AudioFormat::Integer16BitMono:   return AL_FORMAT_MONO16;
			case AudioFormat::Integer8BitStereo:  return AL_FORMAT_STEREO8;
			case AudioFormat::Integer16BitStereo: return AL_FORMAT_STEREO16;

			QK_ASSERT_NO_DEFAULT("Invalid audio format", AL_NONE);
		}
	}
}
