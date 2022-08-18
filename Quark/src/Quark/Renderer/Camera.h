#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const Mat4f& matrix)
			: m_Projection(matrix)
		{
		}

		virtual ~Camera() = default;

		const Mat4f& GetProjection() const { return m_Projection; }

	protected:
		Mat4f m_Projection = Mat4f(1.f);
	};
}
