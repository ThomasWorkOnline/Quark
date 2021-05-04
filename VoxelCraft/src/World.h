#pragma once

#include <Entropy.h>

#include "Chunk.h"
#include "ChunkRenderer.h"
#include "Player.h"

#include <vector>

class World
{
public:
	World();
	~World();

	void OnUpdate(float elapsedTime);

	void AddChunk(Chunk* chunk);
	void RemoveChunk(Chunk* chunk);

	const std::vector<Chunk*>& GetRenderableChunks() const { return m_RenderableChunks; }
	const std::vector<Chunk*>& GetChunks() const { return m_Chunks; }
	std::vector<Chunk*>& GetChunks() { return m_Chunks; }
	Chunk* GetChunk(const glm::ivec2& position) const;

	Entropy::Scene& GetScene() { return m_Scene; }
	Player& GetPlayer() { return m_Player; }
	uint32_t GetRenderDistance() const { return m_RenderDistance; }

	std::tuple<BlockId, glm::ivec3, glm::ivec3> RayCast(const glm::vec3& start, const glm::vec3& direction, float length);

	void ReplaceBlockFromPositionAbsolute(const glm::ivec3& position, BlockId type);
	BlockId GetBlockFromPositionAbsolute(const glm::ivec3& position) const;
	glm::ivec2 GetChunkCoordFromPositionAbsolute(const glm::ivec3& position) const;

private:
	Entropy::Scene m_Scene;
	Player m_Player = { m_Scene };
	uint32_t m_RenderDistance = 16;

	std::vector<Chunk*> m_RenderableChunks;
	std::vector<Chunk*> m_Chunks;

	std::future<void> m_WorldGenFuture;
};
