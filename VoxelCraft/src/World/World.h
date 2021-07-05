#pragma once

#include <Quark.h>

#include "Chunk.h"
#include "ChunkLoader.h"
#include "WorldMap.h"
#include "../Game/Collision.h"
#include "../Game/Player.h"
#include "../Game/PlayerController.h"
#include "../Utils/ConvertPosition.h"

#include <optional>

class World
{
public:
	World();
	~World();

	void OnUpdate(float elapsedTime);
	void OnEvent(Quark::Event& e);

	void OnChunkLoaded(size_t id);
	void OnChunkModified(size_t id);

	const WorldMap& GetMap() const { return m_Map; }
	WorldMap& GetMap() { return m_Map; }
	Block GetBlock(const Position3D& position) const;

	bool IsPlayerTouchingGround(const Player& player) const;

	static Quark::Scope<World> Create();

private:
	bool OnKeyPressed(Quark::KeyPressedEvent& e);
	bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e);

	// Utilities
	std::optional<CollisionData> RayCast(const glm::vec3& start, const glm::vec3& direction, float length) const;
	void ReplaceBlock(const Position3D& position, Block type);
	void ProcessPlayerCollision();

	Quark::Scene m_Scene;
	Player m_Player = { *this, m_Scene };
	PlayerController m_Controller = { m_Player };
	WorldMap m_Map = { *this };

	Quark::Scope<ChunkLoader> m_Loader;
};