#include "qkpch.h"
#include "AudioSource.h"

#include "Platform/Standalone/OpenAL/OpenALAudioSource.h"

namespace Quark {

	Ref<AudioSource> AudioSource::Create()
	{
		return CreateRef<OpenALAudioSource>();
	}
}
