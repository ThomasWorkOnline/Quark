#pragma once

#include "Quark/Audio/AudioBuffer.h"

namespace Quark {

	class OpenALAudioBuffer : public AudioBuffer
	{
	public:
		OpenALAudioBuffer(const AudioBufferSpecification& spec, const void* data);
		~OpenALAudioBuffer() override;

		uint32_t GetBufferID() const override { return m_BufferID; }

	private:
		uint32_t m_BufferID = 0;
		AudioBufferSpecification m_Spec;
	};
}
