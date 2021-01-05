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
		CameraController(float aspectRatio);

		void OnEvent(Event& e);
		void OnUpdate(float elapsedTime);
		void OnResize(float aspectRatio);

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
