#include "qkpch.h"
#include "AudioSource.h"

#include "Platform/OpenAL/OpenALAudioSource.h"

namespace Quark {

	AudioSource* AudioSource::Create()
	{
		return new OpenALAudioSource();
	}
}
