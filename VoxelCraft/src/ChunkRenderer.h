#pragma once

#include "Chunk.h"
#include "Block.h"
#include "World.h"

class ChunkRenderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer operator= (const ChunkRenderer& other) = delete;

	static const std::unordered_map<BlockId, BlockProperties>& GetBlockProperties();

	static void Initialize();
	static void Shutdown();

	static void Submit(const Quark::Ref<Quark::VertexArray>& va);

	static const Quark::RenderStats& GetStats() { return s_Stats; }

private:
	static Quark::RenderStats s_Stats;
};
