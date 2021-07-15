#pragma once

#include <Quark.h>

#include "../Game/Hitbox.h"

namespace VoxelCraft {

	struct MeshProperties
	{
		const glm::vec3* VertexPositions;
		HitBox Hitbox;

		static inline MeshProperties Create(const glm::vec3* vertexPositions, const HitBox& hitbox)
		{
			return {
				vertexPositions,
				hitbox
			};
		}
	};
}
