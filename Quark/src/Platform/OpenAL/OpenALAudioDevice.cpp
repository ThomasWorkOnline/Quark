#include "qkpch.h"
#include "OpenALAudioDevice.h"

#include "OpenALCore.h"
#include <AL/alc.h>

namespace Quark {

	OpenALAudioOutputDevice::OpenALAudioOutputDevice(const char* deviceName)
	{
		QK_PROFILE_FUNCTION();

		m_Device = ALCALL(alcOpenDevice(deviceName));
		ALCboolean extension = ALCALL(alcIsExtensionPresent(m_Device, "ALC_ENUMERATE_ALL_EXT"));

		if (extension)
		{
			m_DeviceName = ALCALL(alcGetString(m_Device, ALC_ALL_DEVICES_SPECIFIER));
		}
		else
		{
			m_DeviceName = ALCALL(alcGetString(m_Device, ALC_DEVICE_SPECIFIER));
		}

		QK_CORE_ASSERT(m_DeviceName, "Could not get output device name!");

		m_Context = AudioContext::Create(m_Device);
		m_Context->Init();
	}

	OpenALAudioOutputDevice::~OpenALAudioOutputDevice()
	{
		QK_PROFILE_FUNCTION();

		m_Context.reset();
		ALCALL(alcCloseDevice(m_Device));
	}
}
