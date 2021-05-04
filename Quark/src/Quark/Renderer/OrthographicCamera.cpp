#include "OrthographicCamera.h"

namespace Quark {

	OrthographicCamera::OrthographicCamera(float aspectRatio, float zoom)
		: m_AspectRatio(aspectRatio), m_Zoom(zoom)
	{
		RecalculateProjection();
	}

	void OrthographicCamera::OnUpdate()
	{
		if (m_RequiresUpdate)
		{
			RecalculateProjection();
			m_RequiresUpdate = false;
		}
	}

	void OrthographicCamera::RecalculateProjection()
	{
		m_Projection = glm::ortho(-m_Zoom * m_AspectRatio, m_Zoom * m_AspectRatio, -m_Zoom, m_Zoom, -1.0f, 1.0f);
	}
}
