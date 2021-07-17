#pragma once

#include <Quark.h>

#include "Chunk.h"
#include "ChunkLoader.h"
#include "WorldMap.h"
#include "../Entity/Player.h"
#include "../Game/Collision.h"
#include "../Game/PlayerController.h"
#include "../Utils/Position.h"

#include <optional>

namespace VoxelCraft {

	class World
	{
	public:
		World();
		~World();

		void OnUpdate(float elapsedTime);
		void OnEvent(Quark::Event& e);

		void OnChunkLoaded(ChunkIdentifier id);
		void OnChunkModified(ChunkIdentifier id);

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

		// TODO: move
		Player m_Player = { *this, m_Scene };
		PlayerController m_Controller = { m_Player };

		WorldMap m_Map = { *this };

		Quark::Scope<ChunkLoader> m_Loader;
	};
}
