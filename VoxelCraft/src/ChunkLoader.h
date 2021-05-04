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

/// <summary>
/// The worker node responsable for the maintaining of the
/// chunk loading queue.
/// </summary>
class ChunkLoader
{
public:
	ChunkLoader(World* world)
		: m_World(world) {}

	~ChunkLoader();

	void Queue(glm::ivec2 coord);
	void Dispose(glm::ivec2 coord);

	const std::unordered_map<size_t, Chunk*>& GetChunks() const;
	Chunk* GetChunk(const glm::ivec2& coord);

	static Quark::Scope<ChunkLoader> Create(World* world);

private:
	void OnLoad(glm::ivec2 coord);
	void OnUnload(glm::ivec2 coord);

	Chunk* GenerateChunk(glm::ivec2 coord);

	std::stack<std::future<void>> m_LoadingQueue;
	std::stack<std::future<void>> m_UnloadingQueue;

	std::unordered_map<size_t, Chunk*> m_LoadedChunks;
	World* m_World;
};
