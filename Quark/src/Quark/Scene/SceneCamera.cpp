#include "qkpch.h"
#include "SceneCamera.h"

namespace Quark {

	void SceneCamera::Resize(float width, float height)
	{
		m_Width = width;
		m_Height = height;
		CalculateProjection();
	}

	void SceneCamera::SetOrthographic(float width, float height, float near, float far)
	{
		m_Model = ProjectionModel::Orthographic;

		m_Width = width;
		m_Height = height;
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
			float aspectRatio = (float)m_Width / (float)m_Height;
			m_Projection = glm::infinitePerspective(glm::radians(m_PerspectiveFov), aspectRatio, m_PerspectiveNearPlane);
		}
		else
		{
			float left   = -m_Width * 0.5f;
			float right  =  m_Width * 0.5f;
			float bottom = -m_Height * 0.5f;
			float top    =  m_Height * 0.5f;

			m_Projection = glm::ortho(left, right, bottom, top, m_OrthoNear, m_OrthoFar);
		}
	}
}
