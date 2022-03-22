#pragma once

#include "../Core/Core.h"

namespace Quark {

	class AudioDevice
	{
	public:
		virtual ~AudioDevice() = default;

		virtual void* GetNativeDevice() const = 0;

		static Scope<AudioDevice> Create();
		static Scope<AudioDevice> Create(std::string_view deviceName);
	};
}
