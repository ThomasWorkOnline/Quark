#pragma once

#include <Quark.h>

namespace VoxelCraft {

	struct MeshProperties
	{
		const glm::vec3* VertexPositions;

		static MeshProperties Create(const glm::vec3* vertexPositions)
		{
			return {
				vertexPositions
			};
		}
	};
}
