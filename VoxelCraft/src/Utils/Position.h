#pragma once

#include <Quark.h>

#include "../World/ChunkIdentifier.h"

namespace VoxelCraft {

	/// <summary>
	/// Extensions used to convert between world and chunk spaces
	/// </summary>
	class Position3D : public glm::vec3
	{
	public:
		Position3D()
			: glm::vec3(0.f) {}

		Position3D(uint32_t x, uint32_t y, uint32_t z)
			: glm::vec3(x, y, z) {}

		Position3D(int32_t x, int32_t y, int32_t z)
			: glm::vec3(x, y, z) {}

		Position3D(float x, float y, float z)
			: glm::vec3(x, y, z) {}

		Position3D(const glm::ivec3& pos)
			: glm::vec3(pos) {}

		Position3D(const glm::vec3& pos)
			: glm::vec3(pos) {}

		Position3D ToWorldSpace(const ChunkCoord& coord) const;
		Position3D ToChunkSpace(const ChunkCoord& coord) const;

		ChunkCoord ToChunkCoord() const;
	};

	class Position2D : public glm::vec2
	{
	public:
		Position2D()
			: glm::vec2(0.f) {}

		Position2D(float x, float y)
			: glm::vec2(x, y) {}

		Position2D(const glm::ivec2& pos)
			: glm::vec2(pos) {}

		Position2D(const glm::vec2& pos)
			: glm::vec2(pos) {}

		Position2D ToWorldSpace(const ChunkCoord& coord) const;
		Position2D ToChunkSpace(const ChunkCoord& coord) const;

		ChunkCoord ToChunkCoord() const;
	};
}
