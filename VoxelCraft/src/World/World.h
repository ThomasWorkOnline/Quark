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
		WorldMap Map = { *this };

		World();
		~World();

		void OnUpdate(float elapsedTime);
		void OnEvent(Quark::Event& e);

		void OnChunkLoaded(ChunkIdentifier id);

		Block GetBlock(const IntPosition3D& position) const;

		bool IsPlayerTouchingGround(const Player& player) const;

		static Quark::Scope<World> Create();

	private:
		bool OnKeyPressed(Quark::KeyPressedEvent& e);
		bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e);

		// Utilities
		std::optional<CollisionData> RayCast(const Position3D& start, const glm::vec3& direction, float length) const;
		void ReplaceBlock(const IntPosition3D& position, Block type);
		void ProcessPlayerCollision();

	private:
		Quark::Scene m_Scene;

		// TODO: move
		Player m_Player = { *this, m_Scene };
		PlayerController m_Controller = { m_Player };

		Quark::Scope<ChunkLoader> m_Loader;
	};
}
