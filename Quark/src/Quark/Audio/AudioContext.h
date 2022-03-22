#pragma once

#include "../Core/Core.h"

namespace Quark {

	class AudioContext
	{
	public:
		virtual ~AudioContext() = default;

		virtual void Init() = 0;

		static Scope<AudioContext> Create(void* device);
	};
}
