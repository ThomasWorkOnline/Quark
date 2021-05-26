#pragma once

#include <Quark.h>

enum class BlockFace : uint8_t
{
	Front = 0,
	Right,
	Back,
	Left,
	Top,
	Bottom
};

struct BlockVertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	float Intensity;
};

glm::ivec3 GetFaceNormal(BlockFace facing);

enum class Blocks : int8_t
{
	None = -1,

	Air = 0,
	Bedrock,
	Dirt,
	GrassBlock,
	Stone,
	Cobblestone
};
