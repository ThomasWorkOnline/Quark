#pragma once

#include "Chunk.h"

namespace VoxelCraft {

	/// <summary>
	/// Thread safe factory object responsible for chunk generation
	/// </summary>
	class ChunkBuilder
	{
	public:
		static void BuildTerrain(const Quark::Ref<Chunk>& data);
		static void BuildMesh(const Quark::Ref<Chunk>& data, const ChunkNeighbors& neighbors);
	};
}
