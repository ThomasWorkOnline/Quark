#pragma once

#include <vector>

#include "Chunk.h"

class World
{
public:
	World();
	~World();

	const std::vector<Chunk>& GetChunks() const { return m_Chunks; }
	std::vector<Chunk>& GetChunks() { return m_Chunks; }

	const Chunk* GetChunk(const glm::ivec2& position) const;
	Entropy::Scene& GetScene() { return m_Scene; }

	void OnUpdate(float elapsedTime);
	void OnRender(const Entropy::Ref<Entropy::Shader>& shader);

private:
	void InitializeChunksAsync();

	Entropy::Scene m_Scene;

	std::vector<Chunk> m_Chunks;

	std::vector<std::future<void>> m_ChunksConstructDataFutures;
	std::vector<std::future<void>> m_ChunksConstructMeshFutures;

	std::future<void> m_WorldGenFuture;

	uint32_t m_RenderDistance = 16;

	Entropy::Ref<Entropy::Texture2D> m_Stone = Entropy::Texture2D::Create("assets/textures/stone.png");
};
