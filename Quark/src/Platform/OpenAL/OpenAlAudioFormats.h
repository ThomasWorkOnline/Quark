#pragma once

#include "../../Quark/Core/Core.h"
#include "../../Quark/Audio/AudioFormats.h"

#include <AL/al.h>
#include <AL/alc.h>

namespace Quark {

	constexpr ALenum GetAudioFormat(AudioFormat format)
	{
		switch (format)
		{
			case Quark::AudioFormat::Mono8:    return AL_FORMAT_MONO8;
			case Quark::AudioFormat::Mono16:   return AL_FORMAT_MONO16;
			case Quark::AudioFormat::Stereo8:  return AL_FORMAT_STEREO8;
			case Quark::AudioFormat::Stereo16: return AL_FORMAT_STEREO16;
		}

		QK_FATAL("Invalid audio format");
		return AL_NONE;
	}
}
