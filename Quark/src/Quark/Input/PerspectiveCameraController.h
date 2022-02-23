#pragma once

#include "../Renderer/PerspectiveCamera.h"

#include "../Events/ApplicationEvent.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/WindowEvents.h"

#include "../Scene/Entity.h"

namespace Quark {

	class PerspectiveCameraController
	{
	public:
		PerspectiveCameraController() = default;
		PerspectiveCameraController(Entity camera);

		/// <summary>
		/// Propagates the events to the controller.
		/// Make sure the camera entity has all `Transform3DComponent`, `PhysicsComponent` and `PerspectiveCameraComponent` components.
		/// </summary>
		/// <param name="e">Event to propagate</param>
		void OnEvent(Event& e);

		/// <summary>
		/// Updates the logic for controlling the bound camera.
		/// Make sure the camera entity has all `Transform3DComponent`, `PhysicsComponent` and `PerspectiveCameraComponent` components.
		/// </summary>
		/// <param name="elapsedTime">Time elapsed between the last update</param>
		void OnUpdate(float elapsedTime);

		bool HasCamera() const { return m_CameraEntity; }
		void AttachCamera(Entity camera) { m_CameraEntity = camera; }
		void DetachCamera() { m_CameraEntity = Entity(); }

	protected:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizedEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);

		// Has to be of generic type Entity to have access to various components
		Entity m_CameraEntity;

		/// <summary>
		/// Members can be altered.
		/// </summary>
		Float m_RollSensitivity = 1.0f;
		Float m_MovementSpeed = 12.0f;
		Float m_MouseSensitivity = 0.002f;
		Float m_MouseScrollSensitivity = 1.0f;

	private:
		Float m_ZoomSpeed = 0.0f;
		Float m_DefaultMovementSpeed = m_MovementSpeed;
		Float m_DefaultRollSensitivity = m_RollSensitivity;
	};
}
