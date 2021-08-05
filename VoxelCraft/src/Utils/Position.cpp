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
		return { (int32_t)std::floor(x / (double)ChunkSpecification::Width), (int32_t)std::floor(z / (double)ChunkSpecification::Depth) };
	}


	IntPosition3D IntPosition3D::ToWorldSpace(const ChunkCoord& coord) const
	{
		return { x + coord.x * ChunkSpecification::Width, y, z + coord.y * ChunkSpecification::Depth };
	}

	IntPosition3D IntPosition3D::ToChunkSpace(const ChunkCoord& coord) const
	{
		return { x - coord.x * ChunkSpecification::Width, y, z - coord.y * ChunkSpecification::Depth };
	}

	ChunkCoord IntPosition3D::ToChunkCoord() const
	{
		return { (int32_t)std::floor(x / (double)ChunkSpecification::Width), (int32_t)std::floor(z / (double)ChunkSpecification::Depth) };
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
		return { (int32_t)std::floor(x / (double)ChunkSpecification::Width), (int32_t)std::floor(y / (double)ChunkSpecification::Depth) };
	}


	IntPosition2D IntPosition2D::ToWorldSpace(const ChunkCoord& coord) const
	{
		return { x + coord.x * ChunkSpecification::Width, y + coord.y * ChunkSpecification::Depth };
	}

	IntPosition2D IntPosition2D::ToChunkSpace(const ChunkCoord& coord) const
	{
		return { x - coord.x * ChunkSpecification::Width, y - coord.y * ChunkSpecification::Depth };
	}

	bool IntPosition2D::IsInBounds() const
	{
		return true;
	}

	ChunkCoord IntPosition2D::ToChunkCoord() const
	{
		return { (int32_t)std::floor(x / (double)ChunkSpecification::Width), (int32_t)std::floor(y / (double)ChunkSpecification::Depth) };
	}
}
