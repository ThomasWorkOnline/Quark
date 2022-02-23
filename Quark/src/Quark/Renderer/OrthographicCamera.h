#pragma once

#include "../Core/Core.h"
#include "Camera.h"

namespace Quark {

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float aspectRatio, float zoom);

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; m_RequiresUpdate = true; }

		float GetZoom() const { return m_Zoom; }
		void SetZoom(float value) { m_Zoom = value; m_RequiresUpdate = true; }

		const glm::mat4& GetMatrix() const { return m_Matrix; }

		void OnUpdate();

	private:
		void RecalculateProjection();

		float m_AspectRatio;
		float m_Zoom;
	};
}
