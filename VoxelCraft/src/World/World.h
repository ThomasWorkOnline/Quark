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

		static Quark::Scope<World> Create(uint32_t renderDistance = 8, const ChunkCoord& loaderAnchor = { 0, 0 });

		World(uint32_t renderDistance, const ChunkCoord& loaderAnchor);

		void OnUpdate(float elapsedTime);
		void SetChunkLoadedCallback(ChunkLoadedCallback callback) { m_ChunkLoadedCallback = callback; }

		Block GetBlock(const IntPosition3D& position) const;
		void ReplaceBlock(const IntPosition3D& position, Block type);
		std::optional<CollisionData> RayCast(const Position3D& start, const glm::vec3& direction, float length) const;

		bool IsPlayerTouchingGround(const Player& player) const;

	private:
		void OnChunkLoaded(ChunkIdentifier id);

	private:
		ChunkLoadedCallback m_ChunkLoadedCallback;

		friend class ChunkLoader;
	};
}
