#pragma once

#include "OrthographicCamera.h"

#include "../Events/ApplicationEvent.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"

#include "../Scene/Entity.h"

namespace Quark {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController() = default;
		OrthographicCameraController(Entity & camera);
		virtual ~OrthographicCameraController() = default;

		/// <summary>
		/// Propagates the events to the controller.
		/// Make sure the camera entity has all `Transform3DComponent`, `PhysicsComponent` and `OrthographicCameraComponent` components.
		/// This method can be overriden.
		/// </summary>
		/// <param name="e">Event to propagate</param>
		virtual void OnEvent(Event & e);

		/// <summary>
		/// Updates the logic for controlling the bound camera.
		/// Make sure the camera entity has all `Transform3DComponent`, `PhysicsComponent` and `OrthographicCameraComponent` components.
		/// This method can be overriden.
		/// </summary>
		/// <param name="elapsedTime">Time elapsed between the last update</param>
		virtual void OnUpdate(float elapsedTime);

		bool HasCamera() const { return m_CameraEntity; }
		void AttachCamera(Entity camera) { m_CameraEntity = camera; }
		void DetachCamera() { m_CameraEntity = Entity(); }

	protected:
		bool OnMouseScrolled(MouseScrolledEvent & e);
		bool OnWindowResized(WindowResizedEvent & e);
		bool OnMouseMoved(MouseMovedEvent & e);

		// Has to be of generic type Entity to have access to various components
		Entity m_CameraEntity;

		/// <summary>
		/// Members can be altered.
		/// </summary>
		double m_RollSensitivity = 1.0f;
		double m_MovementSpeed = 12.0f;
		double m_MouseSensitivity = 0.002f;
		double m_MouseScrollSensitivity = 1.0f;
	};
}
