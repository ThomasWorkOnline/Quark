#pragma once

#include "Chunk.h"
#include "World.h"

#include <atomic>
#include <future>
#include <mutex>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <vector>

struct ChunkLoaderStats
{
	uint32_t ChunksAllocated = 0;
	uint32_t ChunksTerrainGenerated = 0;
	uint32_t ChunksMeshGenerated = 0;
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

	void Queue(glm::ivec2 coord);
	void Dispose(glm::ivec2 coord);

	bool Idling() const;

	const std::unordered_map<size_t, Chunk*>& GetLoadedChunks() const;
	Chunk* GetLoadedChunk(glm::ivec2 coord);
	Chunk* GetDataGeneratedChunk(glm::ivec2 coord);

	const ChunkLoaderStats& GetStats() const { return m_Stats; }

	static Quark::Scope<ChunkLoader> Create(World* world);

private:
	void OnLoad(glm::ivec2 coord);
	void OnUnload(glm::ivec2 coord);

	Chunk* UniqueChunkAllocator(glm::ivec2 coord);
	Chunk* UniqueChunkDataGenerator(glm::ivec2 coord);
	Chunk* UniqueChunkMeshGenerator(glm::ivec2 coord);

	ChunkLoaderStats m_Stats;

	std::unordered_map<size_t, std::future<void>> m_LoadingQueue;
	std::unordered_map<size_t, std::future<void>> m_UnloadingQueue;

	std::unordered_map<size_t, Chunk*> m_FinishedChunks;

	std::unordered_map<size_t, Chunk*> m_AllocatedChunks;
	std::unordered_map<size_t, Chunk*> m_DataGeneratedChunks;
	std::unordered_map<size_t, Chunk*> m_MeshGeneratedChunks;

	World* m_World;
};
