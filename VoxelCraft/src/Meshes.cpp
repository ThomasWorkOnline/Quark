#include "Meshes.h"

void MeshGenerator::Create(const MeshOutputParameters& out, const Position3D& position, Block type, const Chunk* center, const ChunkNeighbors& neighbors)
{
	auto& blockProperties = Resources::GetBlockProperties(type);

	switch (blockProperties.Mesh)
	{
	case MeshModel::Block:
		// Create all the faces
		for (uint8_t f = 0; f < 6; f++)
		{
			if (!center->IsBlockFaceVisible(position, BlockFace(f), neighbors))
				continue;

			MeshGenerator::CreateBlockFaceMesh(out, position.ToWorldSpace(center->GetCoord()), blockProperties, BlockFace(f));
		}
		break;
	case MeshModel::CrossSprite:
		MeshGenerator::CreateCrossSpriteMesh(out, position.ToWorldSpace(center->GetCoord()), blockProperties);
		break;
	}
}

void MeshGenerator::CreateBlockFaceMesh(const MeshOutputParameters& out, const glm::vec3& position, const BlockProperties& props, BlockFace face)
{
	static auto& meshProperties = Resources::GetMeshProperties(MeshModel::Block);

	for (uint8_t i = 0; i < 4; i++)
	{
		Vertex v = {
			meshProperties.VertexPositions[i + static_cast<uint8_t>(face) * 4] + position,
			props.Faces[static_cast<uint8_t>(face)].GetCoords()[i],
			(static_cast<uint8_t>(face) + 1) / 6.0f
		};
		out.Vertices.push_back(v);
	}

	out.Indices.push_back(0 + out.VertexCount);
	out.Indices.push_back(1 + out.VertexCount);
	out.Indices.push_back(3 + out.VertexCount);
	out.Indices.push_back(1 + out.VertexCount);
	out.Indices.push_back(2 + out.VertexCount);
	out.Indices.push_back(3 + out.VertexCount);

	out.VertexCount += 4;
}

void MeshGenerator::CreateCrossSpriteMesh(const MeshOutputParameters& out, const glm::vec3& position, const BlockProperties& props)
{
	static auto& meshProperties = Resources::GetMeshProperties(MeshModel::CrossSprite);

	for (uint8_t x = 0; x < 4; x++)
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			Vertex v = {
				meshProperties.VertexPositions[x * 4 + i] + position,
				props.Faces[0].GetCoords()[i],
				1.0f
			};
			out.Vertices.push_back(v);
		}

		out.Indices.push_back(0 + out.VertexCount);
		out.Indices.push_back(1 + out.VertexCount);
		out.Indices.push_back(3 + out.VertexCount);
		out.Indices.push_back(1 + out.VertexCount);
		out.Indices.push_back(2 + out.VertexCount);
		out.Indices.push_back(3 + out.VertexCount);

		out.VertexCount += 4;
	}
}
