#include "Meshes.h"

void BlockMesh::CreateFace(std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices, uint32_t& outVertexCount,
	const glm::vec3& position, const BlockProperties& props, BlockFace face)
{
	static auto& meshProperties = Resources::GetMeshProperties(MeshModel::Block);

	for (uint8_t i = 0; i < 4; i++)
	{
		Vertex v = {
			meshProperties.VertexPositions[i + static_cast<uint8_t>(face) * 4] + position,
			props.Faces[static_cast<uint8_t>(face)].GetCoords()[i],
			(static_cast<uint8_t>(face) + 1) / 6.0f
		};
		outVertices.push_back(v);
	}

	outIndices.push_back(0 + outVertexCount);
	outIndices.push_back(1 + outVertexCount);
	outIndices.push_back(3 + outVertexCount);
	outIndices.push_back(1 + outVertexCount);
	outIndices.push_back(2 + outVertexCount);
	outIndices.push_back(3 + outVertexCount);

	outVertexCount += 4;
}

void CrossSpriteMesh::Create(std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices, uint32_t& outVertexCount,
	const glm::vec3& position, const BlockProperties& props)
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
			outVertices.push_back(v);
		}

		outIndices.push_back(0 + outVertexCount);
		outIndices.push_back(1 + outVertexCount);
		outIndices.push_back(3 + outVertexCount);
		outIndices.push_back(1 + outVertexCount);
		outIndices.push_back(2 + outVertexCount);
		outIndices.push_back(3 + outVertexCount);

		outVertexCount += 4;
	}
}
