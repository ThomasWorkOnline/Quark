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
		CameraController(Entity camera);

		void OnEvent(Event& e);
		void OnUpdate(float elapsedTime);
		void OnResize(float aspectRatio);

		Entity& GetCamera() { return m_CameraEntity; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);

		Entity m_CameraEntity;

		float m_RollSensitivity = 1.0f;
		float m_MovementSpeed = 12.0f;
		float m_MouseSensitivity = 0.002f;
		float m_MouseScrollSensitivity = 12.0f;
	};
}
