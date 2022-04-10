#include "OpenALAudioDevice.h"

#include "OpenALCore.h"
#include <AL/alc.h>

namespace Quark {

	OpenALAudioOutputDevice::OpenALAudioOutputDevice()
	{
		m_Device = ALCALL(alcOpenDevice(nullptr));

		m_Context = AudioContext::Create(m_Device);
		m_Context->Init();
	}

	OpenALAudioOutputDevice::OpenALAudioOutputDevice(std::string_view deviceName)
	{
		m_Device = ALCALL(alcOpenDevice(deviceName.data()));

		m_Context = AudioContext::Create(m_Device);
		m_Context->Init();
	}

	OpenALAudioOutputDevice::~OpenALAudioOutputDevice()
	{
		m_Context.reset();
		ALCALL(alcCloseDevice(m_Device));
	}

	const char* OpenALAudioOutputDevice::GetDeviceName() const
	{
		const char* name = nullptr;
		ALCALL(bool extension = alcIsExtensionPresent(m_Device, "ALC_ENUMERATE_ALL_EXT"));

		if (extension)
		{
			name = ALCALL(alcGetString(m_Device, ALC_ALL_DEVICES_SPECIFIER));
		}
		else
		{
			name = ALCALL(alcGetString(m_Device, ALC_DEVICE_SPECIFIER));
		}

		QK_CORE_ASSERT(name, "Could not get audio device name!");
		return name;
	}
}
