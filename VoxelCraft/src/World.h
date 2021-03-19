#pragma once

#include <Entropy.h>

#include "Chunk.h"
#include "ChunkRenderer.h"

#include <vector>

class World
{
public:
	World();
	~World();

	const std::vector<Chunk>& GetChunks() const { return m_Chunks; }
	std::vector<Chunk>& GetChunks() { return m_Chunks; }

	const Chunk* GetChunk(const glm::ivec2& position) const;
	Chunk* GetChunk(const glm::ivec2& position);
	const Block* GetBlockFromPositionAbsolute(const glm::ivec3& position) const;
	void ReplaceBlockFromPositionAbsolute(const glm::ivec3& position, BlockType type);

	Entropy::Scene& GetScene() { return m_Scene; }

	std::tuple<const Block*, glm::ivec3, glm::ivec3> RayCast(const glm::vec3& start, const glm::vec3& direction, float length);

	void OnUpdate(float elapsedTime);

private:
	void InitializeChunksAsync();

	Entropy::Scene m_Scene;

	std::vector<Chunk> m_Chunks;

	std::vector<std::future<void>> m_ChunksConstructDataFutures;
	std::vector<std::future<void>> m_ChunksConstructMeshFutures;

	std::future<void> m_WorldGenFuture;

	uint32_t m_RenderDistance = 16;
};
