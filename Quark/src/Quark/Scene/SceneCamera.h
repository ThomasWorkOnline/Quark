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

		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);
		void SetOrthographic(float size, float near = -1.0f, float far = 1.0f);
		void SetPerspective(float fov, float nearPlane = 0.1f);

	private:
		void CalculateProjection();

	private:
		ProjectionModel m_Model{};
		float m_AspectRatio = 0.0f;

		float m_PerspectiveFov = 0.0f;
		float m_PerspectiveNearPlane = 0.1f;

		float m_OrthoSize = 1.0f;
		float m_OrthoNear = -1.0f, m_OrthoFar = 1.0f;
	};
}
