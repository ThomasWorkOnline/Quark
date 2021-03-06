#pragma once

#include "../Core/Core.h"

namespace Entropy {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float aspectRatio, float zoom);

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; m_RequiresUpdate = true; }

		float GetZoom() const { return m_Zoom; }
		void SetZoom(float value) { m_Zoom = value; m_RequiresUpdate = true; }

		const glm::mat4& GetMatrix() const { return m_Projection; }

		void PollUpdate();

	private:
		void RecalculateProjection();

		glm::mat4 m_Projection;
		float m_AspectRatio;
		float m_Zoom;
		bool m_RequiresUpdate = false;
	};
}
