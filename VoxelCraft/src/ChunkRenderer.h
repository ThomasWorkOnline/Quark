#pragma once

#include <Entropy.h>

#include "Chunk.h"
#include "Blocks.h"

#include <unordered_map>

class ChunkRenderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer operator= (const ChunkRenderer& other) = delete;

	static const std::unordered_map<BlockId, BlockProperties>& GetBlockProperties();

	static void Initialize();
	static void Shutdown();

	static void SubmitChunk(const Chunk& chunk);
private:

};
