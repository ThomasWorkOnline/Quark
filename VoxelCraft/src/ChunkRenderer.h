#pragma once

#include <Entropy.h>

#include "Chunk.h"

#include <unordered_map>

struct RendererData
{
	glm::ivec2 SubTextureSize = { 16, 16 };

	Entropy::Ref<Entropy::Shader> Shader;
	Entropy::Ref<Entropy::Texture2D> Texture;
};

class ChunkRenderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer operator= (const ChunkRenderer& other) = delete;

	static const RendererData& GetData();

	static void Initialize();
	static void Shutdown();

	static void SubmitChunk(const Chunk& chunk);

private:

};
