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
		CameraController() = default;
		CameraController(Entity& camera);

		void OnEvent(Event& e);
		void OnUpdate(float elapsedTime);
		void OnResize(float aspectRatio);

		bool HasCamera() const { return m_CameraEntity; }
		void AttachCamera(Entity& camera) { m_CameraEntity = &camera; }
		void DetachCamera() { m_CameraEntity = nullptr; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);

		// Has to be of generic type Entity to have access to various components
		Entity* m_CameraEntity = nullptr;

		float m_RollSensitivity = 1.0f;
		float m_MovementSpeed = 12.0f;
		float m_MouseSensitivity = 0.002f;
		float m_MouseScrollSensitivity = 12.0f;
	};
}
