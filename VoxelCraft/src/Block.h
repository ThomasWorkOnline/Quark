#pragma once

#include <Quark.h>

enum class BlockFace
{
	Front = 0,
	Right,
	Back,
	Left,
	Top,
	Bottom
};

glm::ivec3 GetFaceNormal(BlockFace facing);

enum class Block : uint8_t
{
	Air = 0,
	Bedrock,
	Dirt,
	GrassBlock,
	Stone,
	Cobblestone,
	Poppy,
	Grass
};
