#include "OrthographicCamera.h"

namespace Entropy {

	OrthographicCamera::OrthographicCamera(float aspectRatio, float zoom)
		: m_AspectRatio(aspectRatio), m_Zoom(zoom)
	{
		RecalculateProjection();
	}

	void OrthographicCamera::RecalculateProjection()
	{
		m_Projection = glm::ortho(-m_Zoom * m_AspectRatio, m_Zoom * m_AspectRatio, -m_Zoom, m_Zoom, -500.0f, 500.0f);
		m_RequiresUpdate = false;
	}

	void OrthographicCamera::PollUpdate()
	{
		if (m_RequiresUpdate)
			RecalculateProjection();
	}
}
