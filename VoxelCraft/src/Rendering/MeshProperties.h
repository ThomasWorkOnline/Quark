#pragma once

#include <Quark.h>

#include "../Utils/Position.h"

namespace VoxelCraft {

	struct MeshProperties
	{
		const Position3D* VertexPositions;

		static MeshProperties Create(const Position3D* vertexPositions)
		{
			return {
				vertexPositions
			};
		}
	};
}
