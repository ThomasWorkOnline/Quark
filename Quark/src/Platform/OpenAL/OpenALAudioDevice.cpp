#include "OpenALAudioDevice.h"

#include <AL/alc.h>

namespace Quark {

	OpenALAudioDevice::OpenALAudioDevice()
	{
		m_Device = alcOpenDevice(nullptr);

		m_Context = AudioContext::Create(m_Device);
		m_Context->Init();
	}

	OpenALAudioDevice::OpenALAudioDevice(std::string_view deviceName)
	{
		m_Device = alcOpenDevice(deviceName.data());

		m_Context = AudioContext::Create(m_Device);
		m_Context->Init();
	}

	OpenALAudioDevice::~OpenALAudioDevice()
	{
		alcCloseDevice(m_Device);
	}
}
