#pragma once

#include "../Core/Core.h"
#include "AudioFormats.h"

namespace Quark {

	struct AudioBufferSpecification
	{
		AudioFormat Format;
		size_t Size;
		size_t Frequency;
	};

	class AudioBuffer
	{
	public:
		virtual ~AudioBuffer() = default;

		virtual uint32_t GetAudioID() const = 0;

		static Ref<AudioBuffer> Create(const AudioBufferSpecification& spec, const void* data);
	};
}
