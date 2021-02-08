#include "Camera.h"

namespace Entropy {

	Camera::Camera(float aspectRatio, float fov)
		: m_AspectRatio(aspectRatio), m_Fov(fov)
	{
		RecalculateProjection();
	}

	void Camera::PollUpdate()
	{
		if (m_RequiresUpdate)
			RecalculateProjection();
	}

	void Camera::RecalculateProjection()
	{
		if (std::isnan(m_AspectRatio)) m_AspectRatio = 0.0f;
		m_Projection = glm::infinitePerspective(glm::radians(m_Fov), m_AspectRatio, 0.1f);
		m_RequiresUpdate = false;
	}
}
