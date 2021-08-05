#pragma once

#include <Quark.h>

#include "ChunkIdentifier.h"

#include "../Blocks/Block.h"
#include "../Utils/Position.h"
#include "../Rendering/ChunkMesh.h"
#include "../Rendering/Vertex.h"

namespace VoxelCraft {

#define CHUNK_COORD(id) *reinterpret_cast<const ChunkCoord*>(&id)

	struct ChunkSpecification
	{
		static const int32_t Width = 16;   // x
		static const int32_t Height = 256; // y
		static const int32_t Depth = 16;   // z

		static constexpr uint32_t BlockCount = Width * Height * Depth;
	};

	class World;
	class Chunk;

	struct ChunkNeighbors
	{
		// Order is important
		ChunkNeighbors(const Chunk* north, const Chunk* south, const Chunk* west, const Chunk* east)
			: North(north), South(south), West(west), East(east) {}

		const Chunk* North;
		const Chunk* South;
		const Chunk* West;
		const Chunk* East;
	};

	class Chunk
	{
	public:
		Chunk(World& world, ChunkIdentifier id);
		~Chunk();

		void Save() const;

		ChunkID GetID() const { return m_Id.ID; }
		ChunkCoord GetCoord() const { return m_Id.Coord; }

		Block GetBlock(const IntPosition3D& position) const;
		bool ReplaceBlock(const IntPosition3D& position, Block type);

		const ChunkMesh& GetMesh() const { return m_Mesh; }

		enum class LoadStatus : int8_t
		{
			Allocated = 0,
			WorldGenerating,
			WorldGenerated,
			Loading,
			Loaded,

			Occupied = WorldGenerating | Loading
		};

		LoadStatus GetLoadStatus() const { return m_LoadingStatus; }
		void SetLoadStatus(LoadStatus status) { m_LoadingStatus = status; }

		void BuildTerrain();
		void BuildMesh(const ChunkNeighbors& neighbors);

		/// <summary>
		/// This function must be invoked from the main thread.
		/// </summary>
		void UploadMesh();

		bool IsBlockFaceVisible(const IntPosition3D& position, BlockFace face, const ChunkNeighbors& neighbors) const;

	private:
		Block GenerateBlock(const IntPosition3D& position);
		bool IsBlockTransparent(const IntPosition3D& position) const;

	private:
		std::atomic_bool m_Pushed = false;
		std::atomic_bool m_Edited = false;
		std::atomic<LoadStatus> m_LoadingStatus = LoadStatus::Allocated;

		ChunkIdentifier m_Id;

		ChunkMesh m_Mesh;

		Block* m_Blocks = nullptr;
		int32_t* m_HeightMap = nullptr;

		World& m_World;
	};
}
