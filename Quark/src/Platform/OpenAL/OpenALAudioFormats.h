#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Audio/AudioFormats.h"

#include <AL/al.h>

namespace Quark {

	inline constexpr ALenum s_OpenALFormatLUT[] = {
		/*None              */ AL_NONE,
		/*Integer8BitMono	*/ AL_FORMAT_MONO8,
		/*Integer16BitMono	*/ AL_FORMAT_MONO16,
		/*Integer8BitStereo	*/ AL_FORMAT_STEREO8,
		/*Integer16BitStereo*/ AL_FORMAT_STEREO16,
		/*Float32BitMono	*/ AL_NONE,
		/*Float32BitStereo	*/ AL_NONE
	};

	constexpr ALenum AudioFormatToOpenALFormat(AudioFormat format)
	{
		return s_OpenALFormatLUT[static_cast<size_t>(format)];
	}
}
