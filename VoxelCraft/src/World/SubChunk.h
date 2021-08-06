#pragma once

#include <Quark.h>

#include "../Rendering/ChunkMesh.h"

namespace VoxelCraft {

	struct SubChunkSpecification
	{
		static constexpr int32_t Width  = 16;
		static constexpr int32_t Height = 16;
		static constexpr int32_t Depth  = 16;

		static constexpr uint32_t BlockCount = Width * Height * Depth;
	};

	class Chunk;

	class SubChunk
	{
	public:
		Chunk* Parent = nullptr;
		ChunkMesh Mesh;

		SubChunk(Chunk* parent, uint32_t stackPosition);

		void RebuildMesh(const ChunkNeighbors& neighbors);
		void BuildTerrain();

		bool HasData() const { return m_Data; }

		uint32_t GetHeightOffsetFromParent() const;
		IntPosition3D GetPositionInParentChunk(const IntPosition3D& position) const;
		IntPosition3D GetPositionFromParentChunk(const IntPosition3D& position) const;

	private:
		uint32_t m_StackPosition = 0;
		BlockID m_Data = BlockID::Air;
	};
}
