#pragma once

#include <Quark.h>

#include "Chunk.h"
#include "ChunkLoader.h"
#include "WorldMap.h"
#include "../Entity/Player.h"
#include "../Game/Collision.h"
#include "../Utils/Position.h"

#include <optional>

namespace VoxelCraft {

	class World
	{
	public:
		using ChunkLoadedCallback = std::function<void(ChunkIdentifier)>;

		WorldMap Map = { *this };
		Quark::Scene Scene;
		Quark::Scope<ChunkLoader> Loader;

		World();

		void SetChunkLoadedCallback(ChunkLoadedCallback callback) { m_ChunkLoadedCallback = callback; }

		void OnUpdate(float elapsedTime);
		void OnEvent(Quark::Event& e);

		void OnChunkLoaded(ChunkIdentifier id);

		Block GetBlock(const IntPosition3D& position) const;

		bool IsPlayerTouchingGround(const Player& player) const;

		// Utilities
		std::optional<CollisionData> RayCast(const Position3D& start, const glm::vec3& direction, float length) const;
		void ReplaceBlock(const IntPosition3D& position, Block type);

		static Quark::Scope<World> Create();

	private:
		ChunkLoadedCallback m_ChunkLoadedCallback;
	};
}
