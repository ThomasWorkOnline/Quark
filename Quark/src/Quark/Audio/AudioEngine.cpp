#include "AudioEngine.h"

#include <irrKlang.h>

namespace Quark {

	irrklang::ISoundEngine* AudioEngine::s_Engine;

	void AudioEngine::Initialize()
	{
		QK_TIME_SCOPE_DEBUG(AudioEngine::Initialize);

		s_Engine = irrklang::createIrrKlangDevice();
	}

	void AudioEngine::Dispose()
	{
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