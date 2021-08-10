#include "ChunkFactory.h"

namespace VoxelCraft {

	static std::mutex s_DataGeneratorMutex;
	static std::mutex s_MeshGeneratorMutex;

	void ChunkFactory::BuildTerrain(Chunk* data)
	{
		bool access = false; // Thread safe, function scope access flag
		{
			std::lock_guard<std::mutex> lock(s_DataGeneratorMutex);
			if (data && data->LoadStatus == Chunk::LoadStatus::Allocated)
			{
				access = true;
				data->LoadStatus = Chunk::LoadStatus::WorldGenerating;
			}
		}

		if (access)
		{
			data->BuildTerrain();
			data->LoadStatus = Chunk::LoadStatus::WorldGenerated;
		}
	}

	void ChunkFactory::BuildMesh(Chunk* data, const ChunkNeighbors& neighbors)
	{
		bool access = false;
		{
			std::lock_guard<std::mutex> lock(s_MeshGeneratorMutex);
			if (data && data->LoadStatus == Chunk::LoadStatus::WorldGenerated)
			{
				access = true;
				data->LoadStatus = Chunk::LoadStatus::Loading;
			}
		}

		if (access)
		{
			data->BuildMesh(neighbors);
			data->LoadStatus = Chunk::LoadStatus::Loaded;
		}
	}
}
