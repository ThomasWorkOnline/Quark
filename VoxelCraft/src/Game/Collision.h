#pragma once

#include "../Blocks/Block.h"
#include "../Utils/Position.h"

namespace VoxelCraft {

	struct AABBCollisionResult
	{
		glm::vec3 Overlap;
	};

	struct CollisionData
	{
		Position3D Impact;
		BlockFace Side;
		Block Block;
	};

	struct Ray
	{
		Position3D Origin;
		glm::vec3 Direction;
	};
}
