#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class AudioOutputDevice
	{
	public:
		AudioOutputDevice() = default;
		virtual ~AudioOutputDevice() = default;

		AudioOutputDevice(const AudioOutputDevice&) = delete;
		AudioOutputDevice& operator=(const AudioOutputDevice&) = delete;

		virtual const char* GetDeviceName() const = 0;
		virtual void* GetNativeDevice() const = 0;

		static Scope<AudioOutputDevice> Create();
		static Scope<AudioOutputDevice> Create(std::string_view deviceName);
	};
}
