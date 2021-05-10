#pragma once

#include <Quark.h>

#include "Blocks.h"

struct CollisionData
{
	glm::vec3 Impact;
	BlockFace Side;
	BlockId Block;
};

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;
};