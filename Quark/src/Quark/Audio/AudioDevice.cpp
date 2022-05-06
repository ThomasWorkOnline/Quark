#include "qkpch.h"
#include "AudioDevice.h"

#include "Platform/Standalone/OpenAL/OpenALAudioDevice.h"

namespace Quark {

	Scope<AudioOutputDevice> AudioOutputDevice::Create()
	{
		return CreateScope<OpenALAudioOutputDevice>();
	}

	Scope<AudioOutputDevice> AudioOutputDevice::Create(std::string_view deviceName)
	{
		return CreateScope<OpenALAudioOutputDevice>(deviceName);
	}
}
