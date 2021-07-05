#pragma once

#include <Quark.h>

/// <summary>
/// Extensions used to convert between world and chunk spaces
/// </summary>
class Position3D : public glm::ivec3
{
public:
	Position3D(uint32_t x, uint32_t y, uint32_t z)
		: glm::ivec3(x, y, z) {}

	Position3D(const glm::ivec3& pos)
		: glm::ivec3(pos) {}

	Position3D(const glm::vec3& pos)
		: glm::ivec3(pos) {}

	glm::ivec3 ToWorldSpace(const glm::ivec2& coord) const;
	glm::ivec3 ToChunkSpace(const glm::ivec2& coord) const;

	glm::ivec2 ToChunkCoord() const;
};

class Position2D : public glm::ivec2
{
public:
	Position2D(uint32_t x, uint32_t y)
		: glm::ivec2(x, y) {}

	Position2D(const glm::ivec2& pos)
		: glm::ivec2(pos) {}

	Position2D(const glm::vec2& pos)
		: glm::ivec2(pos) {}

	glm::ivec2 ToWorldSpace(const glm::ivec2& coord) const;
	glm::ivec2 ToChunkSpace(const glm::ivec2& coord) const;

	glm::ivec2 ToChunkCoord() const;
};
