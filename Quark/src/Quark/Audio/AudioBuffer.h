#pragma once

#include "Quark/Core/Core.h"
#include "AudioFormats.h"

namespace Quark {

	struct AudioBufferSpecification
	{
		uint32_t    Samplerate = 0;
		uint32_t    BitDepth = 0;
		uint32_t    Channels = 0;
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

		virtual double GetDuration() const = 0;

		const AudioBufferSpecification& GetSpecification() const { return m_Spec; }

		static Scope<AudioBuffer> Create(std::string_view filepath);
		static Scope<AudioBuffer> Create(const AudioBufferSpecification& spec, const void* data);

	protected:
		AudioBufferSpecification m_Spec{};
	};
}
