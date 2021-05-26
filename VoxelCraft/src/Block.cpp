#include "Block.h"

glm::ivec3 GetFaceNormal(BlockFace facing)
{
	switch (facing)
	{
	case BlockFace::Front:
		return { 0, 0, 1 };
	case BlockFace::Right:
		return { 1, 0, 0 };
	case BlockFace::Back:
		return { 0, 0, -1 };
	case BlockFace::Left:
		return { -1, 0, 0 };
	case BlockFace::Top:
		return { 0, 1, 0 };
	case BlockFace::Bottom:
		return { 0, -1, 0 };
	}
}
