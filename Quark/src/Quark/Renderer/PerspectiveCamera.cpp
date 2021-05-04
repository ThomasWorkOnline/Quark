#include "PerspectiveCamera.h"

namespace Quark {

	PerspectiveCamera::PerspectiveCamera(float aspectRatio, float fov)
		: m_AspectRatio(aspectRatio), m_Fov(fov)
	{
		RecalculateProjection();
	}

	void PerspectiveCamera::OnUpdate()
	{
		if (m_RequiresUpdate)
		{
			RecalculateProjection();
			m_RequiresUpdate = false;
		}
	}

	void PerspectiveCamera::RecalculateProjection()
	{
		if (std::isnan(m_AspectRatio)) m_AspectRatio = 0.0f;
		m_Projection = glm::infinitePerspective(glm::radians(m_Fov), m_AspectRatio, 0.1f);
	}
}
