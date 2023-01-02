#pragma once

#include "Quark/Audio/AudioBuffer.h"

typedef unsigned int ALuint;

namespace Quark {

	class OpenALAudioBuffer final : public AudioBuffer
	{
	public:
		OpenALAudioBuffer(std::string_view filepath);
		OpenALAudioBuffer(const AudioBufferSpecification& spec, const void* data);
		virtual ~OpenALAudioBuffer() final override;

		virtual void SetData(const void* data, size_t size) final override;

		virtual double GetDuration() const final override { return m_Duration; }

		ALuint GetBufferID() const { return m_BufferID; }

		// Non-Copyable
		OpenALAudioBuffer(const OpenALAudioBuffer&) = delete;
		OpenALAudioBuffer& operator=(const OpenALAudioBuffer&) = delete;

	private:
		ALuint m_BufferID = 0;
		double m_Duration = 0.0;
	};
}
