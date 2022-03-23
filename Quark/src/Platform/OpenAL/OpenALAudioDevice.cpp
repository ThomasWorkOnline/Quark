#include "OpenALAudioDevice.h"

#include "OpenALCore.h"

namespace Quark {

	OpenALAudioDevice::OpenALAudioDevice()
	{
		m_Device = ALCALL(alcOpenDevice(nullptr));

		m_Context = AudioContext::Create(m_Device);
		m_Context->Init();
	}

	OpenALAudioDevice::OpenALAudioDevice(std::string_view deviceName)
	{
		m_Device = ALCALL(alcOpenDevice(deviceName.data()));

		m_Context = AudioContext::Create(m_Device);
		m_Context->Init();
	}

	OpenALAudioDevice::~OpenALAudioDevice()
	{
		m_Context.reset();
		ALCALL(alcCloseDevice(m_Device));
	}
}
