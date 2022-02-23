#pragma once

#include "../Core/Core.h"

namespace Quark {

	class Camera
	{
	public:
		Camera(const glm::mat4& matrix = glm::mat4(1.0f))
			: m_Matrix(matrix) {}

	protected:
		glm::mat4 m_Matrix;
		bool m_RequiresUpdate = false;
	};
}
