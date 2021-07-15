#include "Position.h"

#include "../World/Chunk.h"

namespace VoxelCraft {

	Position3D Position3D::ToWorldSpace(const ChunkCoord& coord) const
	{
		return { x + coord.x * ChunkSpecification::Width, y, z + coord.y * ChunkSpecification::Depth };
	}

	Position3D Position3D::ToChunkSpace(const ChunkCoord& coord) const
	{
		return { x - coord.x * ChunkSpecification::Width, y, z - coord.y * ChunkSpecification::Depth };
	}

	ChunkCoord Position3D::ToChunkCoord() const
	{
		return { std::floor(x / (float)ChunkSpecification::Width), std::floor(z / (float)ChunkSpecification::Depth) };
	}

	Position2D Position2D::ToWorldSpace(const ChunkCoord& coord) const
	{
		return { x + coord.x * ChunkSpecification::Width, y + coord.y * ChunkSpecification::Depth };
	}

	Position2D Position2D::ToChunkSpace(const ChunkCoord& coord) const
	{
		return { x - coord.x * ChunkSpecification::Width, y - coord.y * ChunkSpecification::Depth };
	}

	ChunkCoord Position2D::ToChunkCoord() const
	{
		return { std::floor(x / (float)ChunkSpecification::Width), std::floor(y / (float)ChunkSpecification::Depth) };
	}
}
