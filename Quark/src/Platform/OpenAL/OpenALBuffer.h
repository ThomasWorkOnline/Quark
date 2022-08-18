#pragma once

#include "Quark/Audio/AudioBuffer.h"

namespace Quark {

	class OpenALAudioBuffer final : public AudioBuffer
	{
	public:
		OpenALAudioBuffer(std::string_view filepath);
		OpenALAudioBuffer(const AudioBufferSpecification& spec, const void* data);
		virtual ~OpenALAudioBuffer() override;

		virtual void SetData(const void* data, size_t size) override;

		uint32_t GetBufferID() const { return m_BufferID; }

		// Non-Copyable
		OpenALAudioBuffer(const OpenALAudioBuffer&) = delete;
		OpenALAudioBuffer& operator=(const OpenALAudioBuffer&) = delete;

	private:
		uint32_t m_BufferID = 0;
	};
}
