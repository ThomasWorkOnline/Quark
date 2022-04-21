#include "OrthographicCameraController.h"

#include "Quark/Core/Core.h"
#include "Quark/Core/Input.h"
#include "Quark/Core/KeyCodes.h"
#include "Quark/Core/Application.h"

#include "Quark/Scene/Scene.h"
#include "Quark/Scene/Entity.h"
#include "Quark/Scene/Components.h"

#include "Quark/Tools/EasingCurves.h"

namespace Quark {

	OrthographicCameraController::OrthographicCameraController(Entity camera)
		: m_CameraEntity(camera)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep elapsedTime)
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity.GetComponent<Transform3DComponent>();
			auto& physics = m_CameraEntity.GetComponent<PhysicsComponent>();
			auto& camera = m_CameraEntity.GetComponent<OrthographicCameraComponent>().Camera;

			// Boost key
			if (Input::IsKeyPressed(KeyCode::LeftControl))
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
			if (Input::IsKeyPressed(KeyCode::W))
			{
				physics.Velocity += transform.GetTopVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(KeyCode::S))
			{
				physics.Velocity -= transform.GetTopVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(KeyCode::D))
			{
				physics.Velocity += transform.GetRightVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(KeyCode::A))
			{
				physics.Velocity -= transform.GetRightVector() * (Float)(elapsedTime * m_MovementSpeed);
			}

			if (Input::IsKeyPressed(KeyCode::Q))
			{
				transform.Rotate(elapsedTime * m_RollSensitivity, -transform.GetFrontVector());
			}

			if (Input::IsKeyPressed(KeyCode::E))
			{
				transform.Rotate(elapsedTime * m_RollSensitivity, transform.GetFrontVector());
			}

			if (Input::IsKeyPressed(KeyCode::D0))
			{
				physics.Velocity = { 0.0f, 0.0f, 0.0f };
				transform.Position = { 0.0f, 0.0f, 0.0f };
				transform.Orientation = glm::angleAxis<Float>(0.0f, Vector3(0.0f, 0.0f, 1.0f));
				QK_CORE_TRACE("Teleported to world origin");
			}

#if 0
			// Zooming
			static constexpr float zoomFrictionCoeff = 8.0f;
			m_ZoomSpeed -= (m_ZoomSpeed * zoomFrictionCoeff) * elapsedTime;

			// Check if the fov needs to be changed
			if (abs(m_ZoomSpeed) > 0.1f)
			{
				camera.SetZoom(camera.GetZoom() - m_ZoomSpeed * elapsedTime * camera.GetZoom());
			}

			if (camera.GetZoom() < 1.0f)
			{
				camera.SetZoom(1.0f);
				m_ZoomSpeed = 0.0f;
			}
			else if (camera.GetZoom() > 1000.0f)
			{
				camera.SetZoom(1000.0f);
				m_ZoomSpeed = 0.0f;
			}
#endif
		}
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ATTACH_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(OrthographicCameraController::OnWindowResized));
		dispatcher.Dispatch<MouseMovedEvent>(ATTACH_EVENT_FN(OrthographicCameraController::OnMouseMoved));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomSpeed += e.GetYOffset() * m_MouseScrollSensitivity;
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizedEvent& e)
	{
		if (m_CameraEntity)
			m_CameraEntity.GetComponent<OrthographicCameraComponent>().Camera.SetProjection(0, 0, e.GetWidth(), e.GetHeight());

		return false;
	}

	bool OrthographicCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity.GetComponent<Transform3DComponent>();
			auto& camera = m_CameraEntity.GetComponent<OrthographicCameraComponent>().Camera;

			// TODO:
		}

		return false;
	}
}
