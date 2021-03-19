#pragma once

#include <Entropy.h>

#include "Chunk.h"

#include <unordered_map>

struct BlockProperties
{
	bool Transparent = true;
	Entropy::SubTexture2D Texture;
	const char* BreakSound;
};

class ChunkRenderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer operator= (const ChunkRenderer& other) = delete;

	static const std::unordered_map<BlockType, BlockProperties>& GetBlockProperties();

	static void Initialize();
	static void Shutdown();

	static void SubmitChunk(const Chunk& chunk);
private:

};
