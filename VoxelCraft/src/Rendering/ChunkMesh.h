#pragma once

#include <Quark.h>

#include "Vertex.h"
#include "../Blocks/Block.h"
#include "../Utils/Position.h"

namespace VoxelCraft {

	class SubChunk;
	class ChunkNeighbors;
	class BlockProperties;

	class ChunkMesh
	{
	public:
		void Create(const SubChunk& subChunk, const ChunkNeighbors& neighbors);
		void Upload();

		bool Empty() const { return !m_VertexCount; }
		bool Uploaded() const { return m_Uploaded; }

		const Quark::Ref<Quark::VertexArray>& GetVertexArray() const { return m_VertexArray; }

	private:
		void CreateBlockMesh(Block block, const IntPosition3D& position, const SubChunk& subChunk, const ChunkNeighbors& neighbors);
		void CreateBlockFaceMesh(const IntPosition3D& position, const BlockProperties& props, BlockFace face);
		void CreateCrossSpriteMesh(const IntPosition3D& position, const BlockProperties& props);

	private:
		Quark::Ref<Quark::VertexArray> m_VertexArray;

		bool m_Uploaded = true;
		uint32_t m_VertexCount = 0;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}
