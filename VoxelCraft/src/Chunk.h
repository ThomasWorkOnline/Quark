#pragma once

#include <Quark.h>

#include "Blocks.h"

struct ChunkSpecification
{
	static const uint32_t Width  = 16;   // x
	static const uint32_t Height = 256;  // y
	static const uint32_t Depth  = 16;   // z

	static const uint32_t BlockCount = Width * Height * Depth;

	static constexpr glm::vec3 VertexPositions[24] = {
		// front
		{  0.0f,  0.0f,  1.0f },
		{  1.0f,  0.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },
		{  0.0f,  1.0f,  1.0f },

		// right
		{  1.0f,  0.0f,  1.0f },
		{  1.0f,  0.0f,  0.0f },
		{  1.0f,  1.0f,  0.0f },
		{  1.0f,  1.0f,  1.0f },

		// back
		{  1.0f,  0.0f,  0.0f },
		{  0.0f,  0.0f,  0.0f },
		{  0.0f,  1.0f,  0.0f },
		{  1.0f,  1.0f,  0.0f },

		// left
		{  0.0f,  0.0f,  0.0f },
		{  0.0f,  0.0f,  1.0f },
		{  0.0f,  1.0f,  1.0f },
		{  0.0f,  1.0f,  0.0f },

		// top
		{  0.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  0.0f },
		{  0.0f,  1.0f,  0.0f },

		// bottom
		{  1.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f }
	};
};

class World;

class Chunk
{
public:
	Chunk(const glm::ivec2& position, World* world);
	~Chunk();

	const Quark::Ref<Quark::VertexArray>& GetVertexArray() const { return m_VertexArray; }

	const World& GetWorld() const { return *m_World; }
	World& GetWorld() { return *m_World; }

	const glm::ivec2& GetPosition() const { return m_Position; }
	BlockId GetBlock(const glm::ivec3& position) const;

	void ReplaceBlock(const glm::ivec3& position, BlockId type);

	bool DataCreated() const { return m_DataCreated; }
	bool MeshCreated() const { return m_MeshCreated; }
	bool PushData();

	static const ChunkSpecification& GetSpecification();

	void GenerateTerrain();
	void GenerateMesh();

private:
	glm::ivec3 GetBlockPositionAbsolute(const glm::ivec3& position) const;

	void GenerateFaceVertices(const glm::ivec3& position, BlockId type, BlockFace face);
	void GenerateFaceIndices();

	bool IsBlockFaceVisible(const glm::ivec3& position, BlockFace face, Chunk* rightChunk, Chunk* leftChunk, Chunk* frontChunk, Chunk* backChunk) const;
	bool IsBlockOpaque(const glm::ivec3& position) const;

	Quark::Ref<Quark::VertexArray> m_VertexArray;
	Quark::Ref<Quark::VertexBuffer> m_VertexBuffer;
	Quark::Ref<Quark::IndexBuffer> m_IndexBuffer;

	glm::ivec2 m_Position;
	bool m_DataCreated = false;
	bool m_MeshCreated = false;
	bool m_UpdatePending = false;

	uint32_t m_IndexCount = 0;
	uint32_t m_VertexCount = 0;
	std::vector<BlockVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	BlockId* m_Blocks = nullptr;
	World* m_World;
};
