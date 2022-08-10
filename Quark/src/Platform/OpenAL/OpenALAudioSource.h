#pragma once

#include "Quark/Audio/AudioSource.h"

namespace Quark {

	class OpenALAudioSource final : public AudioSource
	{
	public:
		OpenALAudioSource();
		virtual ~OpenALAudioSource() override;

		virtual void SetBuffer(AudioBuffer* buffer) override;

		virtual void Play() override;
		virtual void Stop() override;
		virtual void Pause() override;

	private:
		uint32_t m_SourceID = 0;
	};
}
