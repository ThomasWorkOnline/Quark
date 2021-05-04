#pragma once

#include "../Core/Core.h"

namespace irrklang {
	struct ISoundEngine;
}

namespace Quark {

	class AudioEngine
	{
	public:
		AudioEngine() = delete;
		AudioEngine operator= (const AudioEngine& other) = delete;

		static void Initialize();
		static void Dispose();

		static void PlaySound(const char* filepath);

		static void OnUpdate();

	private:
		static irrklang::ISoundEngine* s_Engine;
	};
}
