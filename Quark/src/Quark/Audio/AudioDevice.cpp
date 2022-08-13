#include "qkpch.h"
#include "AudioDevice.h"

#include "Platform/OpenAL/OpenALAudioDevice.h"

namespace Quark {

	Scope<AudioOutputDevice> AudioOutputDevice::Create(const char* deviceName)
	{
		return CreateScope<OpenALAudioOutputDevice>(deviceName);
	}
}
