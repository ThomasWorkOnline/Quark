#pragma once

#include <Quark.h>

namespace VoxelCraft {

	using ChunkID = size_t;
	using ChunkCoord = glm::ivec2;

	struct ChunkIdentifier
	{
		ChunkIdentifier(int32_t x, int32_t z)
			: Coord(x, z) {}

		ChunkIdentifier(glm::ivec2 coord)
			: Coord(coord) {}

		ChunkIdentifier(ChunkID id)
			: ID(id) {}

		union {
			ChunkID ID;
			ChunkCoord Coord;
		};

		// Coordinate system
		ChunkCoord North() const { return Coord + ChunkCoord( 0,  1); }
		ChunkCoord South() const { return Coord + ChunkCoord( 0, -1); }
		ChunkCoord West()  const { return Coord + ChunkCoord(-1,  0); }
		ChunkCoord East()  const { return Coord + ChunkCoord( 1,  0); }

		bool operator==(ChunkIdentifier other)
		{
			return ID == other.ID;
		}
	};
}
