#include "qkpch.h"
#include "AudioSource.h"

#include "Platform/OpenAL/OpenALAudioSource.h"

namespace Quark {

	Ref<AudioSource> AudioSource::Create()
	{
		return CreateRef<OpenALAudioSource>();
	}
}
