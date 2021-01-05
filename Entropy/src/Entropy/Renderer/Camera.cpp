#include "Camera.h"

namespace Entropy {

	Camera::Camera(float aspectRatio, float fov)
		: m_Fov(fov)
	{
		SetProjectionMatrix(aspectRatio);
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
		if (isnan(m_AspectRatio)) m_AspectRatio = 0.0f;
		m_ProjectionMatrix = glm::infinitePerspective(glm::radians(m_Fov), m_AspectRatio, 0.1f);
	}

	void Camera::SetProjectionMatrix(float aspectRatio)
	{
		m_AspectRatio = aspectRatio;
		m_NeedsProjectionUpdate = true;
	}
}
