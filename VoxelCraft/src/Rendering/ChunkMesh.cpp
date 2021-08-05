#include "ChunkMesh.h"

#include "../Game/Resources.h"
#include "../World/Chunk.h"

namespace VoxelCraft {

	void ChunkMesh::Create(const Chunk* chunk, const ChunkNeighbors& neighbors)
	{
		m_VertexCount = 0;
		m_Vertices.clear();
		m_Indices.clear();

		QK_ASSERT(neighbors.North, "Chunk must be valid");
		QK_ASSERT(neighbors.South, "Chunk must be valid");
		QK_ASSERT(neighbors.West, "Chunk must be valid");
		QK_ASSERT(neighbors.East, "Chunk must be valid");

		Position3D position;
		for (position.y = 0; position.y < ChunkSpecification::Height; position.y++)
		{
			for (position.z = 0; position.z < ChunkSpecification::Depth; position.z++)
			{
				for (position.x = 0; position.x < ChunkSpecification::Width; position.x++)
				{
					Block block = chunk->GetBlock(position);

					if (block == BlockID::Air)
						continue;

					CreateBlockMesh(block, position, chunk, neighbors);
				}
			}
		}
	}

	void ChunkMesh::Upload()
	{
		static const auto& layout = Resources::GetBufferLayout();

		m_VertexArray = Quark::VertexArray::Create();

		auto vbo = Quark::VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
		vbo->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vbo);

		auto ibo = Quark::IndexBuffer::Create(m_Indices.data(), m_Indices.size());
		m_VertexArray->SetIndexBuffer(ibo);
	}

	void ChunkMesh::CreateBlockMesh(Block block, const IntPosition3D& position, const Chunk* chunk, const ChunkNeighbors& neighbors)
	{
		const auto& props = block.GetProperties();

		switch (props.Mesh)
		{
		case BlockModel::Block:
			// Create all the faces
			for (uint8_t f = 0; f < 6; f++)
			{
				if (!chunk->IsBlockFaceVisible(position, BlockFace(f), neighbors))
					continue;

				ChunkMesh::CreateBlockFaceMesh(position.ToWorldSpace(chunk->GetCoord()), props, BlockFace(f));
			}
			break;
		case BlockModel::CrossSprite:
			ChunkMesh::CreateCrossSpriteMesh(position.ToWorldSpace(chunk->GetCoord()), props);
			break;
		}
	}

	void ChunkMesh::CreateBlockFaceMesh(const IntPosition3D& position, const BlockProperties& props, BlockFace face)
	{
		static const auto& meshProperties = Resources::GetMeshProperties(BlockModel::Block);

		for (uint8_t i = 0; i < 4; i++)
		{
			m_Vertices.emplace_back(
				meshProperties.VertexPositions[i + static_cast<uint8_t>(face) * 4] + (Position3D)position,
				props.Faces[static_cast<uint8_t>(face)].GetCoords()[i],
				(static_cast<uint8_t>(face) + 1) / 6.0f
			);
		}

		m_Indices.push_back(0 + m_VertexCount);
		m_Indices.push_back(1 + m_VertexCount);
		m_Indices.push_back(3 + m_VertexCount);
		m_Indices.push_back(1 + m_VertexCount);
		m_Indices.push_back(2 + m_VertexCount);
		m_Indices.push_back(3 + m_VertexCount);

		m_VertexCount += 4;
	}

	void ChunkMesh::CreateCrossSpriteMesh(const IntPosition3D& position, const BlockProperties& props)
	{
		static const auto& meshProperties = Resources::GetMeshProperties(BlockModel::CrossSprite);

		for (uint8_t x = 0; x < 4; x++)
		{
			for (uint8_t i = 0; i < 4; i++)
			{
				m_Vertices.emplace_back(
					meshProperties.VertexPositions[x * 4 + i] + (Position3D)position,
					props.Faces[0].GetCoords()[i],
					1.0f
				);
			}

			m_Indices.push_back(0 + m_VertexCount);
			m_Indices.push_back(1 + m_VertexCount);
			m_Indices.push_back(3 + m_VertexCount);
			m_Indices.push_back(1 + m_VertexCount);
			m_Indices.push_back(2 + m_VertexCount);
			m_Indices.push_back(3 + m_VertexCount);

			m_VertexCount += 4;
		}
	}
}
