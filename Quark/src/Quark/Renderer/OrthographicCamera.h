#pragma once

#include "Camera.h"

namespace Quark {

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float aspectRatio, float zoom);

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjection(); }

		float GetZoom() const { return m_Zoom; }
		void SetZoom(float value) { m_Zoom = value; RecalculateProjection(); }

	private:
		void RecalculateProjection();

		float m_AspectRatio;
		float m_Zoom;
	};
}
