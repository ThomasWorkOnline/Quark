#pragma once

#include "../Core/Core.h"
#include "../Scene/Entity.h"

namespace Entropy {

	class Camera
	{
	public:
		Camera(float aspectRatio, float fov);

		const glm::mat4& GetProjectionMatrix() const { return m_Projection; }

		float GetFov() const { return m_Fov; }
		void SetFov(float fov) { m_Fov = fov; m_RequiresUpdate = true; }

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; m_RequiresUpdate = true; }

		void PollUpdate();

	private:
		void RecalculateProjection();

		glm::mat4 m_Projection;
		float m_AspectRatio;
		float m_Fov;
		bool m_RequiresUpdate = false;
	};
}
