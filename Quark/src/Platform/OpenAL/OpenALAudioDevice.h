#pragma once

#include "../../Quark/Audio/AudioDevice.h"
#include "../../Quark/Audio/AudioContext.h"

typedef struct ALCdevice ALCdevice;

namespace Quark {

	class OpenALAudioDevice : public AudioDevice
	{
	public:
		OpenALAudioDevice();
		OpenALAudioDevice(std::string_view deviceName);
		virtual ~OpenALAudioDevice() override;

		virtual void* GetNativeDevice() const override { return m_Device; }

	private:
		ALCdevice* m_Device;
		Scope<AudioContext> m_Context;
	};
}
