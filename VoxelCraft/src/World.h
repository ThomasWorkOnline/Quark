#pragma once

#include <Quark.h>

#include "Chunk.h"
#include "Collision.h"
#include "ConvertPosition.h"
#include "Player.h"
#include "PlayerController.h"
#include "WorldMap.h"

#include <optional>

class ChunkLoader;

class World
{
public:
	World();
	~World();

	void OnUpdate(float elapsedTime);
	void OnEvent(Quark::Event& e);

	void OnChunkModified(size_t id);

private:
	bool OnKeyPressed(Quark::KeyPressedEvent& e);
	bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e);

	// Utilities
	CollisionData RayCast(const glm::vec3& start, const glm::vec3& direction, float length);
	Block GetBlock(const Position3D& position) const;
	void ReplaceBlock(const Position3D& position, Block type);

	Quark::Scene m_Scene;
	Player m_Player = { m_Scene };
	PlayerController m_Controller = { m_Player };
	WorldMap m_Map;

	Quark::Scope<ChunkLoader> m_Loader;

	friend class ChunkLoader;
};
