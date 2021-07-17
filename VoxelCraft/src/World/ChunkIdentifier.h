#pragma once

#include <Quark.h>

namespace VoxelCraft {

	typedef size_t ChunkID;

	struct ChunkCoord : public glm::ivec2
	{
		ChunkCoord(const glm::ivec2& coord)
			: glm::ivec2(coord) {}

		ChunkCoord(int32_t x, int32_t y)
			: glm::ivec2(x, y) {}

		// Coordinate system
		ChunkCoord North() const { return *this + glm::ivec2( 0,  1); }
		ChunkCoord South() const { return *this + glm::ivec2( 0, -1); }
		ChunkCoord West()  const { return *this + glm::ivec2(-1,  0); }
		ChunkCoord East()  const { return *this + glm::ivec2( 1,  0); }

		ChunkID ToID() const { return *reinterpret_cast<const ChunkID*>(this); }
	};

	struct ChunkIdentifier
	{
		ChunkIdentifier(glm::ivec2 coord)
			: Coord(coord) {}

		ChunkIdentifier(ChunkID id)
			: ID(id) {}

		union {
			ChunkID ID;
			ChunkCoord Coord;
		};

		bool operator==(ChunkIdentifier other)
		{
			return ID == other.ID;
		}
	};
}
