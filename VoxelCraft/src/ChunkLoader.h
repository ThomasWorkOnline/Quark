#pragma once

#include "Chunk.h"
#include "World.h"
#include "LoadingArea.h"

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

class ChunkLoader
{
public:
	ChunkLoader(World* world, const glm::ivec2& coord, uint32_t renderDistance);
	~ChunkLoader();

	void OnUpdate(float elapsedTime);

	void SetCoord(const glm::ivec2& coord) { m_Coord = coord; }
	const glm::ivec2& GetCoord() const { return m_Coord; }

	void Load(glm::ivec2 coord);
	void Unload(glm::ivec2 coord);
	void Rebuild(glm::ivec2 coord);

	bool Idling() const;

	const ChunkLoaderStats& GetStats() const { return m_Stats; }

	static Quark::Scope<ChunkLoader> Create(World* world, const glm::ivec2& coord, uint32_t renderDistance = 8);

private:
	void FlushQueue();

	void OnChunkBorderCrossed();

	void LoadChunk(size_t id);
	void UnloadChunk(size_t id);

	void UniqueChunkDataGenerator(Chunk* chunk);
	void UniqueChunkMeshGenerator(Chunk* chunk, Chunk* left, Chunk* right, Chunk* back, Chunk* front);

	LoadingArea m_LoadingArea;
	
	std::list<size_t> m_LoadingQueue;
	std::list<size_t> m_UnloadingQueue;

	ChunkLoaderStats m_Stats;

	glm::ivec2 m_Coord;
	uint32_t m_RenderDistance;

	World* m_World;
};
