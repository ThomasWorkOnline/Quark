#pragma once

#include <Quark.h>

#include "Chunk.h"

struct ChunkRendererStats
{
	uint32_t DrawCalls = 0;
};

class ChunkRenderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer operator= (const ChunkRenderer& other) = delete;

	static void Initialize();
	static void Shutdown();

	// Debug
	static void SwitchShader();

	static void Submit(const Chunk* chunk);

	static const ChunkRendererStats& GetStats() { return s_Stats; }

private:
	static Quark::Ref<Quark::Shader> s_ActiveShader;

	static ChunkRendererStats s_Stats;
};
