#pragma once

#include <Quark.h>

#include "Block.h"

#define CHUNK_UUID(coord) *reinterpret_cast<size_t*>(&coord)
#define CHUNK_COORD(id) *reinterpret_cast<glm::ivec2*>(&id)

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

	enum class Status : int8_t
	{
		Deallocating = -1,
		Allocated,
		WorldGenerating,
		WorldGenerated,
		Loading,
		Loaded
	};

	Status GetStatus() const { return m_Status.load(); }
	void SetStatus(Status status) { m_Status.store(status); }

private:
	void GenerateWorld();
	void GenerateMesh();
	void PushData();

	glm::ivec3 GetBlockPositionInWorld(const glm::ivec3& position) const;

	void GenerateFaceVertices(const glm::ivec3& position, Block type, BlockFace face);
	void GenerateFaceIndices();

	bool IsBlockFaceVisible(const glm::ivec3& position, BlockFace face) const;
	bool IsBlockOpaque(const glm::ivec3& position) const;

	static Block GenerateBlock(const glm::ivec3& position);

	friend class ChunkLoader;

	Quark::Ref<Quark::VertexArray> m_VertexArray;
	Quark::Ref<Quark::VertexBuffer> m_VertexBuffer;
	Quark::Ref<Quark::IndexBuffer> m_IndexBuffer;

	std::atomic_bool m_Pushed = false;
	std::atomic<Status> m_Status = Status::Allocated;

	glm::ivec2 m_Coord;

	uint32_t m_IndexCount = 0;
	uint32_t m_VertexCount = 0;
	std::vector<BlockVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	Block* m_Blocks = nullptr;
	World* m_World;
};

inline std::ostream& operator<< (std::ostream& os, Chunk::Status status)
{
	switch (status)
	{
	case Chunk::Status::Allocated:
		os << "Allocated";
		break;
	case Chunk::Status::WorldGenerating:
		os << "WorldGenerating";
		break;
	case Chunk::Status::WorldGenerated:
		os << "WorldGenerated";
		break;
	case Chunk::Status::Loading:
		os << "Loading";
		break;
	case Chunk::Status::Loaded:
		os << "Loaded";
		break;
	}

	return os;
}
