#pragma once

#include <Entropy.h>

#include "Block.h"

struct ChunkSpecification
{
	static const uint32_t Width  = 16;   // x
	static const uint32_t Height = 256;  // y
	static const uint32_t Depth  = 16;   // z

	static const uint32_t BlockCount = Width * Height * Depth;

	static constexpr glm::vec3 VertexPositions[24] = {
		// front
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },

		// right
		{  0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f,  0.5f },

		// back
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },

		// left
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f, -0.5f },

		// top
		{ -0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },

		// bottom
		{  0.5f, -0.5f,  0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f }
	};

	static constexpr glm::vec2 VertexTexCoords[4] = {
		{  0.0f,  0.0f },
		{  1.0f,  0.0f },
		{  1.0f,  1.0f },
		{  0.0f,  1.0f },
	};
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	float TexIndex;
	float Intensity;
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
	bool IsGenerated() const { return m_Generated; }

	void ConstructData(const std::atomic<bool>& running);
	void ConstructMesh(const std::atomic<bool>& running);
	void PushData();

	static const ChunkSpecification& GetSpecification();

private:
	bool IsBlockFaceVisible(const glm::ivec3& position, BlockFace face) const;
	bool IsBlockOpaqueAt(const glm::ivec3& position) const;
	const Block* GetBlockAt(const glm::ivec3& position) const;
	glm::ivec3 GetBlockPositionAbsolute(const glm::ivec3& position) const;

	Entropy::Ref<Entropy::VertexArray> m_VertexArray;
	Entropy::Ref<Entropy::VertexBuffer> m_VertexBuffer;
	Entropy::Ref<Entropy::IndexBuffer> m_IndexBuffer;

	glm::ivec2 m_Position;
	bool m_Initialized = false;
	bool m_Generated = false;
	bool m_UpdatePending = false;

	std::vector<Vertex> m_Vertices;
	uint32_t m_IndexCount = 0;
	uint32_t* m_Indices;

	Block* m_Blocks = nullptr;
	World* m_World = nullptr;
};
