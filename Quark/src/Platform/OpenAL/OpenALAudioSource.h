#pragma once

#include "Quark/Audio/AudioSource.h"

typedef unsigned int ALuint;

namespace Quark {

	class OpenALAudioSource final : public AudioSource
	{
	public:
		OpenALAudioSource();
		virtual ~OpenALAudioSource() final override;

		virtual void SetBuffer(const AudioBuffer* buffer) final override;

		virtual void Play() final override;
		virtual void Stop() final override;
		virtual void Pause() final override;

		// Non-Copyable
		OpenALAudioSource(const OpenALAudioSource&) = delete;
		OpenALAudioSource& operator=(const OpenALAudioSource&) = delete;

	private:
		ALuint m_SourceID = 0;
	};
}
