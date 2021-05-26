#pragma once

#include "Chunk.h"
#include "LoadingChunk.h"
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

/// <summary>
/// The worker node responsable for the maintaining of the
/// chunk loading queue.
/// </summary>
class ChunkLoader
{
public:
	ChunkLoader(World* world);
	~ChunkLoader();

	void OnUpdate(float elapsedTime);

	void Load(glm::ivec2 coord);
	void Unload(glm::ivec2 coord);
	void Rebuild(Chunk* chunk);

	bool Idling() const;

	const std::unordered_map<size_t, LoadingChunk*>& GetChunks() const;
	LoadingChunk* GetChunk(glm::ivec2 coord);
	LoadingChunk* GetChunk(size_t id);

	const ChunkLoaderStats& GetStats() const { return m_Stats; }

	static Quark::Scope<ChunkLoader> Create(World* world);

private:
	void FlushQueue();
	void RebuildChunk(Chunk* chunk);
	bool QueueContains(size_t id);

	LoadingChunk* UniqueChunkAllocator(glm::ivec2 coord);
	LoadingChunk* UniqueChunkAllocator(size_t id);
	void UniqueChunkDataGenerator(LoadingChunk* chunk);

	std::unordered_map<size_t, LoadingChunk*> m_Chunks;
	std::list<size_t> m_LoadingQueue;

	ChunkLoaderStats m_Stats;

	World* m_World;
};
