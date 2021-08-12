#pragma once

#include "Chunk.h"
#include "ChunkIdentifier.h"

namespace VoxelCraft {

	/// <summary>
	/// Thread safe factory object responsible for chunk generation
	/// </summary>
	class ChunkFactory
	{
	public:
		static void BuildTerrain(Chunk* data);
		static void BuildMesh(Chunk* data, const ChunkNeighbors& neighbors);
	};
}