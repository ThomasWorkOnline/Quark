#include "AudioDevice.h"

#include "../../Platform/OpenAL/OpenALAudioDevice.h"

namespace Quark {

	Scope<AudioDevice> AudioDevice::Create()
	{
		return CreateScope<OpenALAudioDevice>();
	}

	Scope<AudioDevice> AudioDevice::Create(std::string_view deviceName)
	{
		return CreateScope<OpenALAudioDevice>(deviceName);
	}
}
