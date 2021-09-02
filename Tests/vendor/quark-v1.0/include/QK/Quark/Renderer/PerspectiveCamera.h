#pragma once

#include "../Core/Core.h"

namespace Quark {

	class PerspectiveCamera
	{
	public:
		PerspectiveCamera(float aspectRatio, float fov);

		const glm::mat4& GetProjection() const { return m_Projection; }

		float GetFov() const { return m_Fov; }
		void SetFov(float fov) { m_Fov = fov; m_RequiresUpdate = true; }

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; m_RequiresUpdate = true; }

		void OnUpdate();

	private:
		void RecalculateProjection();

		glm::mat4 m_Projection;
		float m_AspectRatio;
		float m_Fov;
		bool m_RequiresUpdate = false;
	};
}
