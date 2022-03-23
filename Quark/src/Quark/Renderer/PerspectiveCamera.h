#pragma once

#include "Camera.h"

namespace Quark {

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float aspectRatio, float fov);

		float GetFov() const { return m_Fov; }
		void SetFov(float fov) { m_Fov = fov; RecalculateProjection(); }

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjection(); }

	private:
		void RecalculateProjection();

		float m_AspectRatio;
		float m_Fov;
	};
}
