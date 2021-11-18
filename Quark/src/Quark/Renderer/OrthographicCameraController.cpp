#include "OrthographicCameraController.h"

#include "../Core/Core.h"
#include "../Core/Input.h"
#include "../Core/KeyCodes.h"
#include "../Core/Application.h"

#include "../Tools/EasingCurves.h"

#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../Scene/Components.h"

namespace Quark {

	static double s_ZoomSpeed = 0.0f;

	OrthographicCameraController::OrthographicCameraController(Entity camera)
		: m_CameraEntity(camera)
	{
	}

	void OrthographicCameraController::OnUpdate(float elapsedTime)
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity.GetComponent<Transform3DComponent>();
			auto& physics = m_CameraEntity.GetComponent<PhysicsComponent>();
			auto& camera = m_CameraEntity.GetComponent<OrthographicCameraComponent>().Camera;

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
				physics.Velocity += transform.GetTopVector() * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::S))
			{
				physics.Velocity -= transform.GetTopVector() * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::D))
			{
				physics.Velocity += transform.GetRightVector() * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::A))
			{
				physics.Velocity -= transform.GetRightVector() * elapsedTime * m_MovementSpeed;
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
				transform.Orientation = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				QK_CORE_TRACE("Teleported to world origin");
			}

			// Zooming
			constexpr double zoomFrictionCoeff = 8.0f;
			s_ZoomSpeed -= (s_ZoomSpeed * zoomFrictionCoeff) * elapsedTime;

			// Check if the fov needs to be changed
			if (abs(s_ZoomSpeed) > 0.1f)
			{
				camera.SetZoom(camera.GetZoom() - s_ZoomSpeed * elapsedTime * camera.GetZoom());
			}

			if (camera.GetZoom() < 1.0f)
			{
				camera.SetZoom(1.0f);
				s_ZoomSpeed = 0.0f;
			}

			if (camera.GetZoom() > 1000.0f)
			{
				camera.SetZoom(1000.0f);
				s_ZoomSpeed = 0.0f;
			}

			camera.OnUpdate();
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
		s_ZoomSpeed += e.GetYOffset() * m_MouseScrollSensitivity;
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizedEvent& e)
	{
		if (m_CameraEntity)
			m_CameraEntity.GetComponent<OrthographicCameraComponent>().Camera.SetAspectRatio((float)e.GetWidth() / e.GetHeight());

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
