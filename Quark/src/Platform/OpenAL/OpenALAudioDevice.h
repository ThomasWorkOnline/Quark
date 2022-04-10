#pragma once

#include "Quark/Audio/AudioDevice.h"
#include "Quark/Audio/AudioContext.h"

typedef struct ALCdevice ALCdevice;

namespace Quark {

	class OpenALAudioOutputDevice : public AudioOutputDevice
	{
	public:
		OpenALAudioOutputDevice();
		OpenALAudioOutputDevice(std::string_view deviceName);
		~OpenALAudioOutputDevice() override;

		const char* GetDeviceName() const override;
		void* GetNativeDevice() const override { return m_Device; }

	private:
		ALCdevice* m_Device;
		Scope<AudioContext> m_Context;
	};
}
