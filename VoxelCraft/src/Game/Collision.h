#pragma once

#include <Quark.h>

#include "../Blocks/Block.h"

struct AABBCollisionResult
{
	glm::vec3 Overlap;
};

struct CollisionData
{
	glm::vec3 Impact;
	BlockFace Side;
	Block Block;
};

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;
};
