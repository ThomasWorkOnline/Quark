#pragma once

#include <Quark.h>

#include "Chunk.h"
#include "Collision.h"
#include "Player.h"
#include "WorldMap.h"

#include <optional>

typedef class ChunkLoader;

class World
{
public:
	World();
	~World();

	void OnUpdate(float elapsedTime);
	void OnEvent(Quark::Event& e);

	void OnChunkModified(glm::ivec2 coord);

	const Quark::Scene& GetScene() const { return m_Scene; }
	Quark::Scene& GetScene() { return m_Scene; }

	const Player& GetPlayer() const { return m_Player; }
	Player& GetPlayer() { return m_Player; }

	CollisionData RayCast(const glm::vec3& start, const glm::vec3& direction, float length);

	void ReplaceBlock(const glm::ivec3& position, Block type);
	Block GetBlock(const glm::ivec3& position) const;

	static glm::ivec2 GetChunkCoord(const glm::ivec3& position);

private:
	bool OnKeyPressed(Quark::KeyPressedEvent& e);
	bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e);

	Quark::Scene m_Scene;
	Player m_Player = { m_Scene };

	WorldMap m_Map;

	Quark::Scope<ChunkLoader> m_Loader;

	friend class ChunkLoader;
};
