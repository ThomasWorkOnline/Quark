#pragma once

#include "Chunk.h"
#include "World.h"

#include <list>
#include <mutex>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <queue>

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
	ChunkLoader(World* world);
	~ChunkLoader();

	void OnUpdate(float elapsedTime);

	void Load(glm::ivec2 coord);
	void Unload(glm::ivec2 coord);
	void Rebuild(glm::ivec2 coord);

	bool Idling() const;

	const std::unordered_map<size_t, Chunk*>& GetChunks() const;
	Chunk* GetChunk(glm::ivec2 coord);
	Chunk* GetChunk(size_t id);

	const ChunkLoaderStats& GetStats() const { return m_Stats; }

	static Quark::Scope<ChunkLoader> Create(World* world);

private:
	void FlushQueue();
	bool QueueContains(size_t id);
	void RemoveFromQueue(size_t id);

	Chunk* UniqueChunkAllocator(glm::ivec2 coord);
	Chunk* UniqueChunkAllocator(size_t id);
	void UniqueChunkDataGenerator(Chunk* chunk);
	void UniqueChunkMeshGenerator(Chunk* chunk, Chunk* left, Chunk* right, Chunk* back, Chunk* front);

	std::unordered_map<size_t, Chunk*> m_Chunks;
	std::list<size_t> m_LoadingQueue;

	ChunkLoaderStats m_Stats;

	World* m_World;
};
