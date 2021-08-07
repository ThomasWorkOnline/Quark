#pragma once

#include <Quark.h>

#include "Vertex.h"
#include "../Blocks/Block.h"
#include "../Utils/Position.h"

#include <atomic>

namespace VoxelCraft {

	class SubChunk;
	struct BlockProperties;
	struct ChunkNeighbors;

	class ChunkMesh
	{
	public:
		/// <summary>
		/// Generates a sub chunk mesh in chunk space
		/// </summary>
		/// <param name="subChunk"></param>
		/// <param name="neighbors"></param>
		void Create(const SubChunk& subChunk, const ChunkNeighbors& neighbors);
		void Upload();

		bool Empty() const { return !m_VertexCount; }
		bool Uploaded() const { return m_Uploaded; }

		const Quark::Ref<Quark::VertexArray>& GetVertexArray() const { return m_VertexArray; }

	private:
		void CreateBlockMesh(Block block, const IntPosition3D& position, const SubChunk& subChunk, const ChunkNeighbors& neighbors);
		void CreateBlockFaceMesh(const IntPosition3D& position, const BlockProperties& props, BlockFace face);
		void CreateCrossSpriteMesh(const IntPosition3D& position, const BlockProperties& props);

		void CreateQuadIndices();

	private:
		Quark::Ref<Quark::VertexArray> m_VertexArray;

		uint32_t m_VertexCount = 0;
		bool m_Uploaded = true;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}
