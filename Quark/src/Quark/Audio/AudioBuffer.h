#pragma once

#include "Quark/Core/Core.h"
#include "AudioFormats.h"

namespace Quark {

	struct AudioBufferSpecification
	{
		size_t      Size       = 1024;
		uint32_t    Samplerate = 48000;
		AudioFormat Format{};
	};

	class AudioBuffer
	{
	public:
		AudioBuffer() = default;
		AudioBuffer(const AudioBufferSpecification& spec)
			: m_Spec(spec) {}

		virtual ~AudioBuffer() = default;
		virtual void SetData(const void* data, size_t size) = 0;

		const AudioBufferSpecification& GetSpecification() const { return m_Spec; }

		static Ref<AudioBuffer> Create(std::string_view filepath);
		static Ref<AudioBuffer> Create(const AudioBufferSpecification& spec, const void* data);

	protected:
		AudioBufferSpecification m_Spec{};
	};
}
