#include "Camera.h"

namespace Entropy {

	Camera::Camera(uint32_t width, uint32_t height, float fov)
		: m_Fov(fov), m_Width(width), m_Height(height)
	{
		SetProjectionMatrix(width, height);
		RecalculateViewMatrix();
	}

	void Camera::CheckAndUpdate()
	{
		if (m_NeedsViewUpdate)
		{
			RecalculateViewMatrix();
		}

		if (m_NeedsProjectionUpdate)
		{
			RecalculateProjectionMatrix();
		}

		if (m_NeedsViewUpdate || m_NeedsProjectionUpdate)
		{
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
		}

		m_NeedsViewUpdate = false;
		m_NeedsProjectionUpdate = false;
	}

	void Camera::RecalculateViewMatrix()
	{
		m_Orientation = glm::normalize(m_Orientation);
		glm::mat4 rotate = glm::toMat4(m_Orientation);
		m_ViewMatrix = glm::translate(rotate, -m_Position);
	}

	void Camera::RecalculateProjectionMatrix()
	{
		float aspectRatio = (float)m_Width / (float)m_Height;
		if (isnan(aspectRatio)) aspectRatio = 0.0f;
		m_ProjectionMatrix = glm::infinitePerspective(glm::radians(m_Fov), aspectRatio, 0.1f);
	}

	void Camera::SetProjectionMatrix(uint32_t width, uint32_t height)
	{
		m_Width = width; m_Height = height;
		m_NeedsProjectionUpdate = true;
	}
}
