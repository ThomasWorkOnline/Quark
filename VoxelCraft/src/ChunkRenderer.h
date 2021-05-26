#pragma once

#include <Quark.h>

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

	static void Submit(const Quark::Ref<Quark::VertexArray>& va);

	static const ChunkRendererStats& GetStats() { return s_Stats; }

private:
	static ChunkRendererStats s_Stats;
};
