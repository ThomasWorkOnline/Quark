#pragma once

#include <Entropy.h>

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
	Chunk(const glm::ivec2& position, World& world);
	~Chunk();

	const Entropy::Ref<Entropy::VertexArray>& GetVertexArray() const { return m_VertexArray; }
	const glm::ivec2& GetPosition() const { return m_Position; }
	glm::ivec3 GetBlockPositionAbsolute(const glm::ivec3& position) const;
	BlockId GetBlockAt(const glm::ivec3& position) const;

	void ReplaceBlock(const glm::ivec3& position, BlockId type);

	bool Loaded() const { return m_MeshCreated; }
	bool PushData();

	static const ChunkSpecification& GetSpecification();
	static Chunk* Load(glm::ivec2 coord, World* world);

private:
	static void GenerateChunk(glm::ivec2 coord, World* world);
	static void GenerateChunkMesh(Chunk* chunk, World* world);

	void GenerateTerrain(const std::atomic<bool>& running);
	void GenerateMesh(const std::atomic<bool>& running);

	void GenerateFaceVertices(const glm::ivec3& position, BlockId type, BlockFace face);
	void GenerateFaceIndices();

	bool IsBlockFaceVisible(const glm::ivec3& position, BlockFace face, Chunk* rightChunk, Chunk* leftChunk, Chunk* frontChunk, Chunk* backChunk) const;
	bool IsBlockOpaqueAt(const glm::ivec3& position) const;

	Entropy::Ref<Entropy::VertexArray> m_VertexArray;
	Entropy::Ref<Entropy::VertexBuffer> m_VertexBuffer;
	Entropy::Ref<Entropy::IndexBuffer> m_IndexBuffer;

	int32_t m_Id;
	glm::ivec2 m_Position;
	bool m_MeshCreated = false;
	bool m_UpdatePending = false;

	uint32_t m_IndexCount = 0;
	uint32_t m_VertexCount = 0;
	std::vector<BlockVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	BlockId* m_Blocks = nullptr;
	World& m_World;
};
