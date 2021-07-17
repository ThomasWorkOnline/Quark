#pragma once

#include "Vertex.h"
#include "../Blocks/Block.h"
#include "../Utils/Position.h"

namespace VoxelCraft {

	class Chunk;
	class ChunkNeighbors;
	class BlockProperties;

	class ChunkMesh
	{
	public:
		void Create(const Chunk* chunk, const ChunkNeighbors& neighbors);
		void Upload();

		const Quark::Ref<Quark::VertexArray>& GetVertexArray() const { return m_VertexArray; }

	private:
		void CreateBlockMesh(Block block, const Position3D& position, const Chunk* chunk, const ChunkNeighbors& neighbors);
		void CreateBlockFaceMesh(const Position3D& position, const BlockProperties& props, BlockFace face);
		void CreateCrossSpriteMesh(const Position3D& position, const BlockProperties& props);

	private:
		Quark::Ref<Quark::VertexArray> m_VertexArray;

		uint32_t m_VertexCount = 0;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}
