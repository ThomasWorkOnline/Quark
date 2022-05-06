#include "qkpch.h"
#include "AudioContext.h"

#include "Platform/Standalone/OpenAL/OpenALAudioContext.h"

namespace Quark {

	Scope<AudioContext> AudioContext::Create(void* device)
	{
		return CreateScope<OpenALAudioContext>(device);
	}
}
