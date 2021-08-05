#pragma once

#include <Quark.h>

#include "../Utils/Position.h"

namespace VoxelCraft {

	struct MeshProperties
	{
		const IntPosition3D* VertexPositions;

		static MeshProperties Create(const IntPosition3D* vertexPositions)
		{
			return {
				vertexPositions
			};
		}
	};
}
