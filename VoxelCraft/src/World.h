#pragma once

#include <Quark.h>

#include "Block.h"
#include "Chunk.h"
#include "ChunkRenderer.h"
#include "Player.h"
#include "Collision.h"

class ChunkLoader;

class World
{
public:
	World();
	~World();

	void OnUpdate(float elapsedTime);
	void OnEvent(Quark::Event& e);

	Chunk* GetChunk(const glm::ivec2& position) const;

	const Quark::Scene& GetScene() const { return m_Scene; }
	Quark::Scene& GetScene() { return m_Scene; }

	const Player& GetPlayer() const { return m_Player; }
	Player& GetPlayer() { return m_Player; }

	uint32_t GetRenderDistance() const { return m_RenderDistance; }

	CollisionData RayCast(const glm::vec3& start, const glm::vec3& direction, float length);

	void ReplaceBlock(const glm::ivec3& position, BlockId type);
	BlockId GetBlock(const glm::ivec3& position) const;

	glm::ivec2 GetChunkCoord(const glm::ivec3& position) const;

private:
	bool OnKeyPressed(Quark::KeyPressedEvent& e);

	Quark::Scene m_Scene;
	Player m_Player = { m_Scene };
	uint32_t m_RenderDistance = 16;

	Quark::Scope<ChunkLoader> m_Loader;
};
