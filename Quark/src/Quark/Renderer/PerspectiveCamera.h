#pragma once

#include "../Core/Core.h"
#include "Camera.h"

namespace Quark {

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float aspectRatio, float fov);

		const glm::mat4& GetProjection() const { return m_Matrix; }

		float GetFov() const { return m_Fov; }
		void SetFov(float fov) { m_Fov = fov; m_RequiresUpdate = true; }

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; m_RequiresUpdate = true; }

		void OnUpdate();

	private:
		void RecalculateProjection();

		float m_AspectRatio;
		float m_Fov;
	};
}
