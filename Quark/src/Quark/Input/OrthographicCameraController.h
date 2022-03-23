#pragma once

#include "Quark/Events/ApplicationEvent.h"
#include "Quark/Events/MouseEvent.h"
#include "Quark/Events/KeyEvent.h"
#include "Quark/Events/WindowEvents.h"

#include "Quark/Renderer/OrthographicCamera.h"
#include "Quark/Scene/Entity.h"

namespace Quark {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController() = default;
		OrthographicCameraController(Entity camera);

		/// <summary>
		/// Propagates the events to the controller.
		/// Make sure the camera entity has all `Transform3DComponent`, `PhysicsComponent` and `OrthographicCameraComponent` components.
		/// </summary>
		/// <param name="e">Event to propagate</param>
		void OnEvent(Event& e);

		/// <summary>
		/// Updates the logic for controlling the bound camera.
		/// Make sure the camera entity has all `Transform3DComponent`, `PhysicsComponent` and `OrthographicCameraComponent` components.
		/// </summary>
		/// <param name="elapsedTime">Time elapsed between the last update</param>
		void OnUpdate(Timestep elapsedTime);

		bool HasCamera() const { return m_CameraEntity; }
		void AttachCamera(Entity camera) { m_CameraEntity = camera; }
		void DetachCamera() { m_CameraEntity = Entity(); }

	protected:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizedEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);

		Entity m_CameraEntity;

		float m_RollSensitivity = 1.0f;
		float m_MovementSpeed = 12.0f;
		float m_MouseSensitivity = 0.002f;
		float m_MouseScrollSensitivity = 1.0f;

	private:
		float m_ZoomSpeed = 0.0f;
		float m_DefaultMovementSpeed = m_MovementSpeed;
		float m_DefaultRollSensitivity = m_RollSensitivity;
	};
}
