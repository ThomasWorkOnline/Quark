#pragma once

#include "../Core/Core.h"

#include "Camera.h"

#include "../Events/ApplicationEvent.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"

namespace Entropy {

	class CameraController
	{
	public:
		CameraController(uint32_t width, uint32_t height);

		void OnEvent(Event& e);
		void OnUpdate(float elapsedTime);
		void OnResize(uint32_t width, uint32_t height);

		Camera& GetCamera() { return m_Camera; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		Camera m_Camera;

		float m_RollSensitivity = 1.0f;
		float m_MovementSpeed = 12.0f;
		float m_MouseSensitivity = 0.002f;
		float m_MouseScrollSensitivity = 12.0f;
	};
}
