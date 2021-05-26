#pragma once

#include <Quark.h>

#include "Block.h"

struct ChunkSpecification
{
	static const uint32_t Width  = 16;   // x
	static const uint32_t Height = 256;  // y
	static const uint32_t Depth  = 16;   // z

	static constexpr uint32_t BlockCount = Width * Height * Depth;
};

class World;

class Chunk
{
public:
	Chunk(const glm::ivec2& position, World* world);
	~Chunk();

	const Quark::Ref<Quark::VertexArray>& GetVertexArray() const { return m_VertexArray; }

	bool IsPushed() const { return m_Pushed.load(); }

	const World& GetWorld() const { return *m_World; }
	World& GetWorld() { return *m_World; }

	const glm::ivec2& GetCoord() const { return m_Coord; }
	Block GetBlock(const glm::ivec3& position) const;
	void ReplaceBlock(const glm::ivec3& position, Block type);

	void GenerateWorld();
	void GenerateMesh(Chunk* right, Chunk* left, Chunk* front, Chunk* back);

	void PushData();

private:
	glm::ivec3 GetBlockPosition(const glm::ivec3& position) const;

	void GenerateFaceVertices(const glm::ivec3& position, Block type, BlockFace face);
	void GenerateFaceIndices();

	bool IsBlockFaceVisible(const glm::ivec3& position, BlockFace face, Chunk* rightChunk, Chunk* leftChunk, Chunk* frontChunk, Chunk* backChunk) const;
	bool IsBlockOpaque(const glm::ivec3& position) const;

	Quark::Ref<Quark::VertexArray> m_VertexArray;
	Quark::Ref<Quark::VertexBuffer> m_VertexBuffer;
	Quark::Ref<Quark::IndexBuffer> m_IndexBuffer;

	std::atomic_bool m_Pushed = false;

	glm::ivec2 m_Coord;

	uint32_t m_IndexCount = 0;
	uint32_t m_VertexCount = 0;
	std::vector<BlockVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	Block* m_Blocks = nullptr;
	World* m_World;
};
