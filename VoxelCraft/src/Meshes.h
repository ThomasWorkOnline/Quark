#pragma once

#include "Vertex.h"
#include "Chunk.h"
#include "Resources.h"

struct BlockMesh
{
	static void CreateFace(std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices, uint32_t& outVertexCount,
		const glm::vec3& position, const BlockProperties& props, BlockFace face);
};

struct CrossSpriteMesh
{
	static void Create(std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices, uint32_t& outVertexCount,
		const glm::vec3& position, const BlockProperties& props);
};
