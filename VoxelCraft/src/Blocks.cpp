#include "Blocks.h"

glm::ivec3 GetFaceNormal(BlockFace facing)
{
	switch (facing)
	{
	case BlockFace::Front:
		return { 0, 0, 1 };
		break;
	case BlockFace::Right:
		return { 1, 0, 0 };
		break;
	case BlockFace::Back:
		return { 0, 0, -1 };
		break;
	case BlockFace::Left:
		return { -1, 0, 0 };
		break;
	case BlockFace::Top:
		return { 0, 1, 0 };
		break;
	case BlockFace::Bottom:
		return { 0, -1, 0 };
		break;
	default:
		break;
	}
}
