#include "World.h"

#include <future>
#include <chrono>

#include "ChunkLoader.h"
#include "../Game/Resources.h"
#include "../Rendering/Renderer.h"

namespace VoxelCraft {

	Quark::Scope<World> World::Create(uint32_t renderDistance, const ChunkCoord& loaderAnchor)
	{
		return Quark::CreateScope<World>(renderDistance, loaderAnchor);
	}

	World::World(uint32_t renderDistance, const ChunkCoord& loaderAnchor)
		: Map(*this)
	{
		Loader = ChunkLoader::Create(*this, loaderAnchor, renderDistance);
	}

	void World::OnUpdate(float elapsedTime)
	{
		Scene.OnUpdate(elapsedTime);
		Map.OnUpdate(elapsedTime);
	}

	Block World::GetBlock(const IntPosition3D& position) const
	{
		ChunkCoord coord = position.ToChunkCoord();
		IntPosition3D blockPosition = position.ToChunkSpace(coord);

		const auto& chunk = Map.Get(coord);
		if (chunk && chunk->LoadStatus == Chunk::LoadStatus::Loaded)
			return chunk->GetBlock(blockPosition);
		return BlockID::Air;
	}

	void World::ReplaceBlock(const IntPosition3D& position, Block type)
	{
		ChunkCoord coord = position.ToChunkCoord();
		IntPosition3D blockPosition = position.ToChunkSpace(coord);

		const auto& chunk = Map.Get(coord);
		if (chunk && chunk->LoadStatus == Chunk::LoadStatus::Loaded)
		{
			Block oldBlock = chunk->GetBlock(blockPosition);
			if (oldBlock != type)
			{
				if (chunk->ReplaceBlock(blockPosition, type))
				{
					if (oldBlock == BlockID::Air)
					{
						auto& blockProperties = type.GetProperties();
						Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
					}
					else
					{
						auto& blockProperties = oldBlock.GetProperties();
						Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
					}
				}
			}
		}
	}

	// TODO: implement a proper raycast
	std::optional<CollisionData> World::RayCast(const Position3D& start, const glm::vec3& direction, float length) const
	{
		glm::dvec3 normDir = glm::normalize(direction);

		for (double i = 0; i < length; i += 0.01)
		{
			glm::ivec3 position = glm::floor(start + normDir * i);
			Block block = GetBlock(position);
			if (block != BlockID::Air)
			{
				CollisionData data;
				data.Block = block;
				data.Impact = position;
				data.Side = BlockFace::Top;

				return data;
			}
		}
		return {};
	}

	bool World::IsPlayerTouchingGround(const Player& player) const
	{
		static constexpr float detectionTreshold = 0.01f;
		IntPosition3D blockUnderFeetPos = glm::floor(player.GetPosition() - Position3D(0.0f, detectionTreshold, 0.0f));
		auto& props = GetBlock(blockUnderFeetPos).GetProperties();

		return props.CollisionEnabled;
	}

	void World::OnChunkLoaded(ChunkIdentifier id)
	{
		if (m_ChunkLoadedCallback)
			m_ChunkLoadedCallback(id);
	}
}
