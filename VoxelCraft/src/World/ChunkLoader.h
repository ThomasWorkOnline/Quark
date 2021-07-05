#pragma once

#include "Chunk.h"
#include "WorldArea.h"

#include <list>
#include <mutex>
#include <thread>
#include <tuple>

struct ChunkLoaderStats
{
	uint32_t ChunksAllocated = 0;
	uint32_t ChunksFreed = 0;
	uint32_t ChunksWorldGen = 0;
	uint32_t ChunksMeshGen = 0;
};

class World;

class ChunkLoader
{
public:
	ChunkLoader(World& world, const glm::ivec2& coord, uint32_t renderDistance);
	~ChunkLoader();

	void OnUpdate(float elapsedTime);

	void SetCoord(const glm::ivec2& coord) { m_Coord = coord; }
	const glm::ivec2& GetCoord() const { return m_Coord; }

	void Load(size_t id);
	void Unload(size_t id);
	void Rebuild(size_t id);

	bool Idling() const;

	const ChunkLoaderStats& GetStats() const { return m_Stats; }

	static Quark::Scope<ChunkLoader> Create(World& world, const glm::ivec2& coord, uint32_t renderDistance = 8);

private:
	void ProcessQueues();
	void ProcessLoading();
	void ProcessUnloading();

	void LoadChunk(size_t id);
	void UnloadChunk(size_t id);

	void OnChunkBorderCrossed();

	void UniqueChunkDataGenerator(Chunk* chunk);
	void UniqueChunkMeshGenerator(Chunk* chunk, Chunk* left, Chunk* right, Chunk* back, Chunk* front);

	World& m_World;
	WorldArea m_LoadingArea;
	
	std::list<size_t> m_LoadingQueue;
	std::list<size_t> m_UnloadingQueue;

	ChunkLoaderStats m_Stats;

	glm::ivec2 m_Coord;
	uint32_t m_RenderDistance;
};
