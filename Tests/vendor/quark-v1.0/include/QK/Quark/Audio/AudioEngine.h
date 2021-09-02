#pragma once

#include "../Core/Core.h"

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
	};
}
