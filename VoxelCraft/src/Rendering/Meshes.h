#pragma once

#include "Vertex.h"
#include "../World/Chunk.h"

namespace VoxelCraft {

	class BlockProperties;

	struct MeshOutputParameters
	{
		std::vector<Vertex>& Vertices;
		std::vector<uint32_t>& Indices;
		uint32_t& VertexCount;
	};

	class ChunkMesh
	{
	public:
		static void Create(const MeshOutputParameters& out, const Position3D& position, const BlockProperties& props, const Chunk* center, const ChunkNeighbors& neighbors);

	private:
		static void CreateBlockFaceMesh(const MeshOutputParameters& out, const Position3D& position, const BlockProperties& props, BlockFace face);
		static void CreateCrossSpriteMesh(const MeshOutputParameters& out, const Position3D& position, const BlockProperties& props);
	};
}
