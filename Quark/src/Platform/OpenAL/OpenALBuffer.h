#pragma once

#include "../../Quark/Audio/AudioBuffer.h"

namespace Quark {

	class OpenALAudioBuffer : public AudioBuffer
	{
	public:
		OpenALAudioBuffer(const AudioBufferSpecification& spec, const void* data);
		virtual ~OpenALAudioBuffer() override;

		virtual uint32_t GetAudioID() const override { return m_AudioID; }

	private:
		uint32_t m_AudioID = 0;
		AudioBufferSpecification m_Spec;
	};
}
