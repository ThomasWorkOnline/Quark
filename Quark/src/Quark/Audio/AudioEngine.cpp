#include "AudioEngine.h"

#include <irrKlang.h>

namespace Quark {

	static irrklang::ISoundEngine* s_Engine;

	void AudioEngine::Initialize()
	{
		QK_SCOPE_TIMER(AudioEngine::Initialize);

		s_Engine = irrklang::createIrrKlangDevice();
		QK_ASSERT(s_Engine, "Could not initialize the audio device!");
	}

	void AudioEngine::Dispose()
	{
		QK_SCOPE_TIMER(AudioEngine::Dispose);

		s_Engine->drop();
	}

	void AudioEngine::PlaySound(const char* filepath)
	{
		s_Engine->play2D(filepath);
	}

	void AudioEngine::OnUpdate()
	{
		s_Engine->update();
	}
}
