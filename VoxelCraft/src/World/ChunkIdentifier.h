#pragma once

#include <Quark.h>

namespace VoxelCraft {

	typedef size_t ChunkID;

	class ChunkCoord : public glm::ivec2
	{
	public:
		ChunkCoord(const glm::ivec2& coord)
			: glm::ivec2(coord) {}

		ChunkCoord(const glm::vec2& pos)
			: glm::ivec2(pos) {}

		ChunkCoord(int32_t x, int32_t y)
			: glm::ivec2(x, y) {}

		ChunkCoord(float x, float y)
			: glm::ivec2(x, y) {}

		ChunkID ToID() const { return *reinterpret_cast<const ChunkID*>(this); }
	};
}
