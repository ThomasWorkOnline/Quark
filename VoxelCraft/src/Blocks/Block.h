#pragma once

#include "../Utils/Position.h"

namespace VoxelCraft {

	enum class BlockFace
	{
		Front = 0,
		Right,
		Back,
		Left,
		Top,
		Bottom
	};

	struct BlockProperties;

	enum BlockID : uint8_t
	{
		Air = 0,
		Bedrock,
		Dirt,
		GrassBlock,
		Stone,
		Cobblestone,
		Poppy,
		Grass
	};

	struct Block
	{
		BlockID ID;

		Block() = default;
		Block(BlockID id)
			: ID(id) {}

		const BlockProperties& GetProperties() const;

		bool operator==(Block other) const { return ID == other.ID; }
		bool operator!=(Block other) const { return ID != other.ID; }
	};

	IntPosition3D GetFaceNormal(BlockFace facing);
}
