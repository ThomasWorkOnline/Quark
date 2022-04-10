#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class AudioSource
	{
	public:
		virtual ~AudioSource() = default;

		virtual void Play() = 0;
		virtual void Stop() = 0;
		virtual void Pause() = 0;

		virtual void Attach(uint32_t buffer) = 0;

		static Ref<AudioSource> Create();
	};
}
