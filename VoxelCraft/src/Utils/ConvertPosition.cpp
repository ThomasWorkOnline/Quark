#include "ConvertPosition.h"

#include "../World/Chunk.h"

glm::ivec3 Position3D::ToWorldSpace(const glm::ivec2& coord) const
{
	return { x + coord.x * ChunkSpecification::Width, y, z + coord.y * ChunkSpecification::Depth };
}

glm::ivec3 Position3D::ToChunkSpace(const glm::ivec2& coord) const
{
	return { x - coord.x * ChunkSpecification::Width, y, z - coord.y * ChunkSpecification::Depth };
}

glm::ivec2 Position3D::ToChunkCoord() const
{
	return { std::floor(x / (float)ChunkSpecification::Width), std::floor(z / (float)ChunkSpecification::Depth) };
}


glm::ivec2 Position2D::ToWorldSpace(const glm::ivec2& coord) const
{
	return { x + coord.x * ChunkSpecification::Width, y + coord.y * ChunkSpecification::Depth };
}

glm::ivec2 Position2D::ToChunkSpace(const glm::ivec2& coord) const
{
	return { x - coord.x * ChunkSpecification::Width, y - coord.y * ChunkSpecification::Depth };
}

glm::ivec2 Position2D::ToChunkCoord() const
{
	return { std::floor(x / (float)ChunkSpecification::Width), std::floor(y / (float)ChunkSpecification::Depth) };
}
