#pragma once

#include <Quark.h>

#include "../Utils/Position.h"

namespace VoxelCraft {

	struct Vertex
	{
		glm::ivec3 Position;
		glm::vec2 TexCoord;
		float Intensity;

		Vertex(const Position3D& position, const glm::vec2& texCoord, float intensity)
			: Position(position), TexCoord(texCoord), Intensity(intensity) {}
	};
}
