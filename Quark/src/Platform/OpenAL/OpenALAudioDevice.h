#pragma once

#include "Quark/Audio/AudioDevice.h"
#include "Quark/Audio/AudioContext.h"

typedef struct ALCdevice ALCdevice;

namespace Quark {

	class OpenALAudioOutputDevice final : public AudioOutputDevice
	{
	public:
		OpenALAudioOutputDevice(const char* deviceName = nullptr);
		virtual ~OpenALAudioOutputDevice() override;

		virtual const char* GetDeviceName() const override { return m_DeviceName; }
		virtual void* GetNativeDevice() const override { return m_Device; }

	private:
		ALCdevice* m_Device;
		Scope<AudioContext> m_Context;

		const char* m_DeviceName = nullptr;
	};
}
