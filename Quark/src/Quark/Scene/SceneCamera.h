#pragma once

#include "Quark/Renderer/Camera.h"

namespace Quark {

	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionModel
		{
			Perspective = 0,
			Orthographic
		};

		SceneCamera() = default;

		void Resize(float width, float height);
		void SetOrthographic(float width, float height, float near = -1.0f, float far = 1.0f);
		void SetPerspective(float fov, float nearPlane = 0.1f);

		float GetFov() const { return m_PerspectiveFov; }
		void SetFov(float fov) { m_PerspectiveFov = fov; CalculateProjection(); }

	private:
		void CalculateProjection();

	private:
		ProjectionModel m_Model{};

		float m_PerspectiveFov = 0.0f;
		float m_PerspectiveNearPlane = 0.1f;

		float m_Width = 1.0f, m_Height = 1.0f;
		float m_OrthoNear = -1.0f, m_OrthoFar = 1.0f;
	};
}
