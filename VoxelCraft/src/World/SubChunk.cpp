#include "SubChunk.h"

#include "Chunk.h"
#include "World.h"

namespace VoxelCraft {

	SubChunk::SubChunk(Chunk* parent, uint32_t stackPosition)
		: Parent(parent), m_StackPosition(stackPosition) {}

	void SubChunk::RebuildMesh(const ChunkNeighbors& neighbors)
	{
		Mesh.Create(*this, neighbors);
	}

	void SubChunk::BuildTerrain()
	{
		IntPosition3D position;
		for (position.y = 0; position.y < SubChunkSpecification::Height; position.y++)
		{
			for (position.z = 0; position.z < SubChunkSpecification::Depth; position.z++)
			{
				for (position.x = 0; position.x < SubChunkSpecification::Width; position.x++)
				{
					IntPosition3D parentPosition = GetPositionInParentChunk(position);
					Block type = Parent->GenerateBlock(parentPosition);
					Parent->m_Blocks[IndexAtPosition(parentPosition)] = type;

					// Append type
					m_Data = (BlockID)(m_Data | type.ID);
				}
			}
		}
	}

	uint32_t SubChunk::GetHeightOffsetFromParent() const
	{
		return SubChunkSpecification::Height * m_StackPosition;
	}

	IntPosition3D SubChunk::GetPositionInParentChunk(const IntPosition3D& position) const
	{
		uint32_t heightOffset = GetHeightOffsetFromParent();
		return position + IntPosition3D(0, heightOffset, 0);
	}

	IntPosition3D SubChunk::GetPositionFromParentChunk(const IntPosition3D& position) const
	{
		uint32_t heightOffset = GetHeightOffsetFromParent();
		return position - IntPosition3D(0, heightOffset, 0);
	}
}
