#pragma once

#include <Quark.h>

#include "../World/ChunkIdentifier.h"

namespace VoxelCraft {

	/// <summary>
	/// Extensions used to convert between world and chunk spaces
	/// </summary>
	class Position3D : public glm::dvec3
	{
	public:
		constexpr Position3D()
			: glm::dvec3(0.0) {}

		constexpr Position3D(int32_t x, int32_t y, int32_t z)
			: glm::dvec3(x, y, z) {}

		constexpr Position3D(double x, double y, double z)
			: glm::dvec3(x, y, z) {}

		constexpr Position3D(const glm::vec3& pos)
			: glm::dvec3(pos) {}

		constexpr Position3D(const glm::ivec3& pos)
			: glm::dvec3(pos) {}

		constexpr Position3D(const glm::dvec3& pos)
			: glm::dvec3(pos) {}

		Position3D ToWorldSpace(const ChunkCoord& coord) const;
		Position3D ToChunkSpace(const ChunkCoord& coord) const;

		ChunkCoord ToChunkCoord() const;
	};

	class IntPosition3D : public glm::ivec3
	{
	public:
		constexpr IntPosition3D()
			: glm::ivec3(0) {}

		constexpr IntPosition3D(double x, double y, double z)
		: glm::ivec3(x, y, z) {}

		constexpr IntPosition3D(int32_t x, int32_t y, int32_t z)
			: glm::ivec3(x, y, z) {}

		constexpr IntPosition3D(const glm::vec3& pos)
			: glm::ivec3(pos) {}

		constexpr IntPosition3D(const glm::ivec3& pos)
			: glm::ivec3(pos) {}

		constexpr IntPosition3D(const glm::dvec3& pos)
			: glm::ivec3(pos) {}

		IntPosition3D ToWorldSpace(const ChunkCoord& coord) const;
		IntPosition3D ToChunkSpace(const ChunkCoord& coord) const;

		ChunkCoord ToChunkCoord() const;
	};

	class Position2D : public glm::dvec2
	{
	public:
		constexpr Position2D()
			: glm::dvec2(0.0) {}

		constexpr Position2D(int32_t x, int32_t y)
			: glm::dvec2(x, y) {}

		constexpr Position2D(double x, double y)
			: glm::dvec2(x, y) {}

		constexpr Position2D(const glm::vec2& pos)
			: glm::dvec2(pos) {}

		constexpr Position2D(const glm::ivec2& pos)
			: glm::dvec2(pos) {}

		constexpr Position2D(const glm::dvec2& pos)
			: glm::dvec2(pos) {}

		Position2D ToWorldSpace(const ChunkCoord& coord) const;
		Position2D ToChunkSpace(const ChunkCoord& coord) const;

		ChunkCoord ToChunkCoord() const;
	};

	class IntPosition2D : public glm::ivec2
	{
	public:
		constexpr IntPosition2D()
			: glm::ivec2(0) {}

		constexpr IntPosition2D(int32_t x, int32_t y)
			: glm::ivec2(x, y) {}

		constexpr IntPosition2D(double x, double y)
			: glm::ivec2(x, y) {}

		constexpr IntPosition2D(const glm::vec2& pos)
			: glm::ivec2(pos) {}

		constexpr IntPosition2D(const glm::ivec2& pos)
			: glm::ivec2(pos) {}

		constexpr IntPosition2D(const glm::dvec2& pos)
			: glm::ivec2(pos) {}

		IntPosition2D ToWorldSpace(const ChunkCoord& coord) const;
		IntPosition2D ToChunkSpace(const ChunkCoord& coord) const;

		bool IsInBounds() const;

		ChunkCoord ToChunkCoord() const;
	};
}
