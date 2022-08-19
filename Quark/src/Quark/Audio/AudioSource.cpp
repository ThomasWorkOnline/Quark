#include "qkpch.h"
#include "AudioSource.h"

#include "Platform/OpenAL/OpenALAudioSource.h"

namespace Quark {

	Scope<AudioSource> AudioSource::Create()
	{
		return CreateScope<OpenALAudioSource>();
	}
}
