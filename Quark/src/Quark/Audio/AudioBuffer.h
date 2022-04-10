#pragma once

#include "Quark/Core/Core.h"
#include "AudioFormats.h"

namespace Quark {

	struct AudioBufferSpecification
	{
		size_t Size         = 1024;
		uint32_t Samplerate = 48000;
		AudioFormat Format;
	};

	class AudioBuffer
	{
	public:
		virtual ~AudioBuffer() = default;

		virtual uint32_t GetBufferID() const = 0;

		static Ref<AudioBuffer> Create(std::string_view filepath);
		static Ref<AudioBuffer> Create(const AudioBufferSpecification& spec, const void* data);
	};
}
