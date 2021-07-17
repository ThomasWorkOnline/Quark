#pragma once

#include <Quark.h>

#include "../World/Chunk.h"

namespace VoxelCraft {

	struct RendererStats
	{
		uint32_t DrawCalls = 0;
	};

	class Renderer
	{
	public:
		Renderer() = delete;
		Renderer operator= (const Renderer& other) = delete;

		static void Initialize();
		static void Shutdown();

		// Debug
		static void SwitchShader();

		static void SubmitChunk(const Chunk* chunk);
		static void DrawUI(uint32_t width, uint32_t height);

		static const RendererStats& GetStats() { return s_Stats; }

	private:
		static Quark::Ref<Quark::Shader> s_ActiveShader;

		static RendererStats s_Stats;
	};
}
