#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class Camera
	{
	public:
		Camera(const glm::mat4& matrix = glm::mat4(1.0f))
			: m_Projection(matrix) {}
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }

	protected:
		glm::mat4 m_Projection;
	};
}
