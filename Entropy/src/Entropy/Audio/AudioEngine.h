#pragma once

namespace irrklang {
	struct ISoundEngine;
}

namespace Entropy {

	class AudioEngine
	{
	public:
		static void Init();
		static void Dispose();

		static void PlaySound(const char* filepath);

		static void OnUpdate();

	private:
		static irrklang::ISoundEngine* s_Engine;
	};
}
