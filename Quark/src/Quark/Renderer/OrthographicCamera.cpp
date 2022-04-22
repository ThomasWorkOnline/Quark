#include "qkpch.h"
#include "OrthographicCamera.h"

namespace Quark {

	OrthographicCamera::OrthographicCamera()
	{
		SetProjection(-1, 1, -1, 1);
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		SetProjection(left, right, bottom, top);
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	}
}
