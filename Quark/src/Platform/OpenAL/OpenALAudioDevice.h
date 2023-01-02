#pragma once

#include "Quark/Audio/AudioDevice.h"
#include "Quark/Audio/AudioContext.h"

typedef struct ALCdevice ALCdevice;

namespace Quark {

	class OpenALAudioOutputDevice final : public AudioOutputDevice
	{
	public:
		OpenALAudioOutputDevice(std::string_view deviceName);
		virtual ~OpenALAudioOutputDevice() final override;

		virtual const char* GetDeviceName() const final override { return m_DeviceName; }
		virtual void* GetNativeDevice() const final override { return m_Device; }

		// Non-Copyable
		OpenALAudioOutputDevice(const OpenALAudioOutputDevice&) = delete;
		OpenALAudioOutputDevice& operator=(const OpenALAudioOutputDevice&) = delete;

	private:
		ALCdevice* m_Device = nullptr;
		Scope<AudioContext> m_Context;

		const char* m_DeviceName = nullptr;
	};
}
