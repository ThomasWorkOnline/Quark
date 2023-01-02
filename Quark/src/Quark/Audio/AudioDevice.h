#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class AudioOutputDevice
	{
	public:
		virtual ~AudioOutputDevice() = default;

		virtual const char* GetDeviceName() const = 0;
		virtual void* GetNativeDevice() const = 0;

		static Scope<AudioOutputDevice> Create(std::string_view deviceName = {});
	};
}
