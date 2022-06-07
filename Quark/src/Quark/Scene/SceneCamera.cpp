#include "qkpch.h"
#include "SceneCamera.h"

namespace Quark {

	void SceneCamera::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_AspectRatio = (float)viewportWidth / (float)viewportHeight;
		if (std::isnan(m_AspectRatio)) m_AspectRatio = 0.0f;
		CalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float near, float far)
	{
		m_Model = ProjectionModel::Orthographic;

		m_OrthoSize = size;
		m_OrthoNear = near;
		m_OrthoFar  = far;

		CalculateProjection();
	}

	void SceneCamera::SetPerspective(float fov, float nearPlane)
	{
		m_Model = ProjectionModel::Perspective;

		m_PerspectiveFov = fov;
		m_PerspectiveNearPlane = nearPlane;

		CalculateProjection();
	}

	void SceneCamera::CalculateProjection()
	{
		if (m_Model == ProjectionModel::Perspective)
		{
			m_Projection = glm::infinitePerspective(glm::radians(m_PerspectiveFov), m_AspectRatio, m_PerspectiveNearPlane);
		}
		else
		{
			float left   = -m_OrthoSize * m_AspectRatio * 0.5f;
			float right  =  m_OrthoSize * m_AspectRatio * 0.5f;
			float bottom = -m_OrthoSize * 0.5f;
			float top    =  m_OrthoSize * 0.5f;

			m_Projection = glm::ortho(left, right, bottom, top, m_OrthoNear, m_OrthoFar);
		}
	}
}
