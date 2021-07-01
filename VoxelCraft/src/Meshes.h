#pragma once

#include "Vertex.h"
#include "Chunk.h"
#include "Resources.h"

struct MeshOutputParameters
{
	std::vector<Vertex>& Vertices;
	std::vector<uint32_t>& Indices;
	uint32_t& VertexCount;
};

class MeshGenerator
{
public:
	static void Create(const MeshOutputParameters& out, const Position3D& position, Block type, const Chunk* center, const ChunkNeighbors& neighbors);

private:
	static void CreateBlockFaceMesh(const MeshOutputParameters& out, const glm::vec3& position, const BlockProperties& props, BlockFace face);
	static void CreateCrossSpriteMesh(const MeshOutputParameters& out, const glm::vec3& position, const BlockProperties& props);
};
