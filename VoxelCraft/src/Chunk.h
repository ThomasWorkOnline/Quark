#pragma once

#include <Quark.h>

#include "Block.h"

#define CHUNK_UUID(coord) *reinterpret_cast<const size_t*>(&coord)
#define CHUNK_COORD(id) *reinterpret_cast<const glm::ivec2*>(&id)

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
	Chunk(const glm::ivec2& coord, World* world);
	~Chunk();

	const Quark::Ref<Quark::VertexArray>& GetVertexArray() const { return m_VertexArray; }

	void Save() const;

	const World& GetWorld() const { return *m_World; }
	World& GetWorld() { return *m_World; }

	const glm::ivec2& GetCoord() const { return m_Coord; }
	Block GetBlock(const glm::ivec3& position) const;
	void ReplaceBlock(const glm::ivec3& position, Block type);

	enum class LoadStatus : int8_t
	{
		Allocated = 0,
		WorldGenerating,
		WorldGenerated,
		Loading,
		Loaded,

		Occupied = WorldGenerating || Loading
	};

	enum class RenderStatus : int8_t
	{
		Invisible = 0,
		Visible
	};

	LoadStatus GetLoadStatus() const { return m_LoadingStatus; }
	void SetLoadStatus(LoadStatus status) { m_LoadingStatus = status; }

	RenderStatus GetRenderStatus() const { return m_RenderingStatus; }
	void SetRenderStatus(RenderStatus status) { m_RenderingStatus = status; }

private:
	void GenerateWorld();
	void GenerateMesh(Chunk* left, Chunk* right, Chunk* back, Chunk* front, bool ignoreNullNeighbors = false);
	void PushData();

	glm::ivec3 GetBlockPositionInWorld(const glm::ivec3& position) const;

	void GenerateFaceVertices(const glm::ivec3& position, Block type, BlockFace face);
	void GenerateFaceIndices();

	bool IsBlockFaceVisible(const glm::ivec3& position, BlockFace face, Chunk* left, Chunk* right, Chunk* back, Chunk* front, bool ignoreNullNeighbors) const;
	bool IsBlockOpaque(const glm::ivec3& position) const;

	static Block GenerateBlock(const glm::ivec3& position);

	friend class ChunkLoader;

	Quark::Ref<Quark::VertexArray> m_VertexArray;
	Quark::Ref<Quark::VertexBuffer> m_VertexBuffer;
	Quark::Ref<Quark::IndexBuffer> m_IndexBuffer;

	std::atomic_bool m_Pushed = false;
	std::atomic_bool m_Edited = false;
	std::atomic<LoadStatus> m_LoadingStatus = LoadStatus::Allocated;
	std::atomic<RenderStatus> m_RenderingStatus = RenderStatus::Visible;

	glm::ivec2 m_Coord;

	uint32_t m_IndexCount = 0;
	uint32_t m_VertexCount = 0;
	std::vector<BlockVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	Quark::Ref<std::string> m_TestString;

	Block* m_Blocks = nullptr;
	World* m_World;
};

inline std::ostream& operator<< (std::ostream& os, Chunk::LoadStatus status)
{
	switch (status)
	{
	case Chunk::LoadStatus::Allocated:
		os << "Allocated";
		break;
	case Chunk::LoadStatus::WorldGenerating:
		os << "WorldGenerating";
		break;
	case Chunk::LoadStatus::WorldGenerated:
		os << "WorldGenerated";
		break;
	case Chunk::LoadStatus::Loading:
		os << "Loading";
		break;
	case Chunk::LoadStatus::Loaded:
		os << "Loaded";
		break;
	}
	return os;
}
