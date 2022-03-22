#include "AudioContext.h"

#include "../../Platform/OpenAL/OpenALAudioContext.h"

namespace Quark {

	Scope<AudioContext> AudioContext::Create(void* device)
	{
		return CreateScope<OpenALAudioContext>(device);
	}
}
