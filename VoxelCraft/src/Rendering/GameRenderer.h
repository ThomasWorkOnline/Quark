#pragma once

#include <Quark.h>

#include "../World/Chunk.h"

namespace VoxelCraft {

	struct GameRendererStats
	{
		uint32_t DrawCalls = 0;
	};

	class GameRenderer
	{
	public:
		GameRenderer() = delete;
		GameRenderer operator= (const GameRenderer& other) = delete;

		static void Initialize();
		static void Shutdown();

		// Debug
		static void SwitchShader();

		static void SubmitChunk(const Chunk* chunk);
		static void DrawUI(uint32_t width, uint32_t height);

		static const GameRendererStats& GetStats() { return s_Stats; }

	private:
		static Quark::Ref<Quark::Shader> s_ActiveShader;

		static GameRendererStats s_Stats;
	};
}
