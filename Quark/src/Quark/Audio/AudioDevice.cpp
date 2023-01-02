#include "qkpch.h"
#include "AudioDevice.h"

#include "Platform/OpenAL/OpenALAudioDevice.h"

namespace Quark {

	Scope<AudioOutputDevice> AudioOutputDevice::Create(std::string_view deviceName)
	{
		return CreateScope<OpenALAudioOutputDevice>(deviceName);
	}
}
