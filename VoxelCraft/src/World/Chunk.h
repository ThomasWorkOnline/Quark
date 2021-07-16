#pragma once

#include <Quark.h>

#include "ChunkIdentifier.h"

#include "../Blocks/Block.h"
#include "../Utils/Position.h"
#include "../Rendering/Vertex.h"

namespace VoxelCraft {

#define CHUNK_UUID(coord) *reinterpret_cast<const ChunkID*>(&coord)
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
		Chunk* Left;
		Chunk* Right;
		Chunk* Back;
		Chunk* Front;
	};

	class Chunk
	{
	public:
		Chunk(World& world, ChunkID id);
		~Chunk();

		const Quark::Ref<Quark::VertexArray>& GetVertexArray() const { return m_VertexArray; }

		void Save() const;

		const ChunkCoord& GetCoord() const { return m_Coord; }
		ChunkID GetId() const { return m_Id; }

		Block GetBlock(const Position3D& position) const;
		void ReplaceBlock(const Position3D& position, Block type);

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
		void BuildMesh(Chunk* left, Chunk* right, Chunk* back, Chunk* front);

		/// <summary>
		/// This function must be invoked from the main thread.
		/// </summary>
		void PushData();

		bool IsBlockFaceVisible(const Position3D& position, BlockFace face, const ChunkNeighbors& neighbors) const;
		bool IsBlockTransparent(const Position3D& position) const;
	private:
		Block GenerateBlock(const Position3D& position);

	private:
		Quark::Ref<Quark::VertexArray> m_VertexArray;

		std::atomic_bool m_Pushed = false;
		std::atomic_bool m_Edited = false;
		std::atomic<LoadStatus> m_LoadingStatus = LoadStatus::Allocated;

		union {
			ChunkCoord m_Coord;
			ChunkID m_Id;
		};

		uint32_t m_VertexCount = 0;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		Block* m_Blocks = nullptr;
		int32_t* m_HeightMap = nullptr;

		World& m_World;
	};

	inline std::ostream& operator<< (std::ostream& os, Chunk::LoadStatus status)
	{
		switch (status)
		{
		case Chunk::LoadStatus::Allocated:
			os << "Allocated";
			break;
		case Chunk::LoadStatus::WorldGenerating:
			os << "WorldGenerating";
			break;
		case Chunk::LoadStatus::WorldGenerated:
			os << "WorldGenerated";
			break;
		case Chunk::LoadStatus::Loading:
			os << "Loading";
			break;
		case Chunk::LoadStatus::Loaded:
			os << "Loaded";
			break;
		}
		return os;
	}
}
