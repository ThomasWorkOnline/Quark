#pragma once

#include "Quark/Audio/AudioSource.h"

namespace Quark {

	class OpenALAudioSource : public AudioSource
	{
	public:
		OpenALAudioSource();
		~OpenALAudioSource() override;

		void Play() override;
		void Stop() override;
		void Pause() override;

		void Attach(uint32_t buffer) override;

	private:
		uint32_t m_SourceID = 0;
	};
}
