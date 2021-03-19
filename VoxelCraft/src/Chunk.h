#pragma once

#include <Entropy.h>

enum BlockFace
{
	Front = 0,
	Right,
	Back,
	Left,
	Top,
	Bottom
};

enum class BlockType
{
	Air = 0,
	Bedrock,
	Dirt,
	GrassBlock,
	Stone,
	Cobblestone
};

struct Block
{
	BlockType Id = BlockType::Air;
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	float TexIndex;
	float Intensity;
};

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

struct World;

class Chunk
{
public:
	Chunk(const glm::ivec2& position, World* world);
	~Chunk();

	const Entropy::Ref<Entropy::VertexArray>& GetVertexArray() const { return m_VertexArray; }

	const glm::ivec2& GetPosition() const { return m_Position; }

	bool IsInitialized() const { return m_Initialized; }

	void ReplaceBlock(const glm::ivec3& position, BlockType type);
	const Block* GetBlockAt(const glm::ivec3& position) const;

	void ConstructChunkData(const std::atomic<bool>& running);
	void ConstructChunkMesh(const std::atomic<bool>& running);
	void PushData();

	static const ChunkSpecification& GetSpecification();

private:
	void ConstructMeshIndices();

	bool IsBlockFaceVisible(const glm::ivec3& position, BlockFace face) const;
	bool IsBlockOpaqueAt(const glm::ivec3& position) const;
	glm::ivec3 GetBlockPositionAbsolute(const glm::ivec3& position) const;

	Entropy::Ref<Entropy::VertexArray> m_VertexArray;
	Entropy::Ref<Entropy::VertexBuffer> m_VertexBuffer;
	Entropy::Ref<Entropy::IndexBuffer> m_IndexBuffer;

	glm::ivec2 m_Position;
	bool m_Initialized = false;
	bool m_UpdatePending = false;

	uint32_t m_IndexCount = 0;
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	Block* m_Blocks = nullptr;
	World* m_World = nullptr;
};
