#pragma once

#include "Quark/Core/Core.h"
#include "AudioBuffer.h"

namespace Quark {

	class AudioSource
	{
	public:
		virtual ~AudioSource() = default;

		virtual void SetBuffer(const AudioBuffer* buffer) = 0;

		virtual void Play() = 0;
		virtual void Stop() = 0;
		virtual void Pause() = 0;

		static Scope<AudioSource> Create();
	};
}
