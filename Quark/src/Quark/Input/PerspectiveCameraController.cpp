#include "PerspectiveCameraController.h"

#include "../Core/Core.h"
#include "../Core/Input.h"
#include "../Core/KeyCodes.h"
#include "../Core/Application.h"

#include "../Tools/EasingCurves.h"

#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../Scene/Components.h"

namespace Quark {

	PerspectiveCameraController::PerspectiveCameraController(Entity camera)
		: m_CameraEntity(camera)
	{
	}

	void PerspectiveCameraController::OnUpdate(Timestep elapsedTime)
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity.GetComponent<Transform3DComponent>();
			auto& physics = m_CameraEntity.GetComponent<PhysicsComponent>();
			auto& camera = m_CameraEntity.GetComponent<PerspectiveCameraComponent>().Camera;

			// Boost key
			if (Input::IsKeyPressed(Key::LeftControl))
			{
				m_MovementSpeed = 100.0f;
				m_RollSensitivity = 5.0f;
			}
			else
			{
				m_MovementSpeed = m_DefaultMovementSpeed;
				m_RollSensitivity = m_DefaultRollSensitivity;
			}

			// Controls
			if (Input::IsKeyPressed(Key::W))
			{
				physics.Velocity += transform.GetFrontVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(Key::S))
			{
				physics.Velocity -= transform.GetFrontVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(Key::D))
			{
				physics.Velocity += transform.GetRightVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(Key::A))
			{
				physics.Velocity -= transform.GetRightVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(Key::Space))
			{
				physics.Velocity += transform.GetTopVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(Key::LeftShift))
			{
				physics.Velocity -= transform.GetTopVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(Key::Q))
			{
				transform.Rotate(elapsedTime * m_RollSensitivity, -transform.GetFrontVector());
			}

			if (Input::IsKeyPressed(Key::E))
			{
				transform.Rotate(elapsedTime * m_RollSensitivity, transform.GetFrontVector());
			}

			if (Input::IsKeyPressed(Key::D0))
			{
				physics.Velocity = { 0.0f, 0.0f, 0.0f };
				transform.Position = { 0.0f, 0.0f, 0.0f };
				transform.Orientation = glm::angleAxis<Float>(0.0f, Vector3(0.0f, 0.0f, 1.0f));
				QK_CORE_TRACE("Teleported to world origin");
			}

			// Zooming
			static constexpr float zoomFrictionCoeff = 8.0f;
			m_ZoomSpeed -= (m_ZoomSpeed * zoomFrictionCoeff) * elapsedTime;

			// Check if the fov needs to be changed
			if (abs(m_ZoomSpeed) > 0.1f)
			{
				camera.SetFov(camera.GetFov() - m_ZoomSpeed * elapsedTime * camera.GetFov());
			}

			if (camera.GetFov() < 1.0f)
			{
				camera.SetFov(1.0f);
				m_ZoomSpeed = 0.0f;
			}
			else if (camera.GetFov() > 90.0f)
			{
				camera.SetFov(90.0f);
				m_ZoomSpeed = 0.0f;
			}
		}
	}

	void PerspectiveCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ATTACH_EVENT_FN(PerspectiveCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(PerspectiveCameraController::OnWindowResized));
		dispatcher.Dispatch<MouseMovedEvent>(ATTACH_EVENT_FN(PerspectiveCameraController::OnMouseMoved));
	}

	bool PerspectiveCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomSpeed += e.GetYOffset() * m_MouseScrollSensitivity;
		return false;
	}

	bool PerspectiveCameraController::OnWindowResized(WindowResizedEvent& e)
	{
		if (m_CameraEntity)
			m_CameraEntity.GetComponent<PerspectiveCameraComponent>().Camera.SetAspectRatio((float)e.GetWidth() / e.GetHeight());

		return false;
	}

	bool PerspectiveCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity.GetComponent<Transform3DComponent>();
			auto& camera = m_CameraEntity.GetComponent<PerspectiveCameraComponent>().Camera;

			Vector2 mouseMove = { e.GetXOffset(), e.GetYOffset() };
			Quat qYaw = glm::angleAxis(-mouseMove.x * m_MouseSensitivity * camera.GetFov() / 90.0f, Vector3(0.0f, 1.0f, 0.0f) * transform.Orientation);
			Quat qPitch = glm::angleAxis(-mouseMove.y * m_MouseSensitivity * camera.GetFov() / 90.0f, Vector3(1.0f, 0.0f, 0.0f) * transform.Orientation);

			transform.Rotate(qPitch * qYaw);
		}

		return false;
	}
}
