#pragma once

#include "Chunk.h"
#include "LoadingChunk.h"
#include "World.h"

#include <atomic>
#include <future>
#include <mutex>
#include <stack>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>
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

	void Persist(glm::ivec2 coord);
	void Dispose(glm::ivec2 coord);

	bool Idling() const;

	const std::unordered_map<size_t, LoadingChunk*>& GetChunks() const;
	Chunk* GetChunk(glm::ivec2 coord);

	const ChunkLoaderStats& GetStats() const { return m_Stats; }

	static Quark::Scope<ChunkLoader> Create(World* world);

private:
	void FlushQueue();

	//Chunk* UniqueChunkAllocator(glm::ivec2 coord);
	//Chunk* UniqueChunkDataGenerator(glm::ivec2 coord);
	//Chunk* UniqueChunkMeshGenerator(glm::ivec2 coord);

	std::unordered_map<size_t, LoadingChunk*> m_Chunks;

	std::queue<size_t> m_LoadingQueue;

	//std::unordered_map<size_t, Chunk*> m_AllocatedChunks;
	//std::unordered_map<size_t, Chunk*> m_DataGeneratedChunks;
	//std::unordered_map<size_t, Chunk*> m_MeshGeneratedChunks;

	ChunkLoaderStats m_Stats;

	World* m_World;
};
