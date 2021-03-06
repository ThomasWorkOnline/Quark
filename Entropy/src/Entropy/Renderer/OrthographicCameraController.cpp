#include "OrthographicCameraController.h"

#include "../Core/Core.h"
#include "../Core/Input.h"
#include "../Core/KeyCodes.h"
#include "../Core/Application.h"

#include "../Tools/EasingCurves.h"

#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../Scene/Components.h"

namespace Entropy {

	static float s_ZoomSpeed = 0.0f;

	OrthographicCameraController::OrthographicCameraController(Entity& camera)
	{
		m_CameraEntity = &camera;
	}

	void OrthographicCameraController::OnUpdate(float elapsedTime)
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity->GetComponent<TransformComponent>();
			auto& physics = m_CameraEntity->GetComponent<PhysicsComponent>();
			auto& camera = m_CameraEntity->GetComponent<OrthographicCameraComponent>().Camera;

			// Movement
			static const float defaultMovementSpeed = m_MovementSpeed;
			static const float defaultRollSensitivity = m_RollSensitivity;

			// Boost key
			if (Input::IsKeyPressed(Key::LeftControl))
			{
				m_MovementSpeed = 100.0f;
				m_RollSensitivity = 100.0f;
			}
			else
			{
				m_MovementSpeed = defaultMovementSpeed;
				m_RollSensitivity = defaultRollSensitivity;
			}

			// Controls
			if (Input::IsKeyPressed(Key::D))
			{
				physics.Velocity += glm::vec3(1.0f, 0.0f, 0.0f) * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::A))
			{
				physics.Velocity += glm::vec3(-1.0f, 0.0f, 0.0f) * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::Space))
			{
				physics.Velocity += glm::vec3(0.0f, 1.0f, 0.0f) * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::LeftShift))
			{
				physics.Velocity += glm::vec3(0.0f, -1.0f, 0.0f) * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::Q))
			{
				transform.Rotate(elapsedTime * m_RollSensitivity, glm::vec3(0.0f, 0.0f, -1.0f));
			}

			if (Input::IsKeyPressed(Key::E))
			{
				transform.Rotate(elapsedTime * m_RollSensitivity, glm::vec3(0.0f, 0.0f, 1.0f));
			}

			if (Input::IsKeyPressed(Key::D0))
			{
				physics.Velocity = { 0.0f, 0.0f, 0.0f };
				transform.Position = { 0.0f, 0.0f, 0.0f };
				NT_TRACE("Teleported to world origin");
			}

			// Zooming
			constexpr float zoomFrictionCoeff = 8.0f;
			s_ZoomSpeed -= (s_ZoomSpeed * zoomFrictionCoeff) * elapsedTime;

			// Check if the fov needs to be changed
			if (abs(s_ZoomSpeed) > 0.1f)
			{
				camera.SetZoom(camera.GetZoom() - s_ZoomSpeed * elapsedTime * m_MouseScrollSensitivity * camera.GetZoom() / 120.0f);
			}

			if (camera.GetZoom() < 1.0f)
			{
				camera.SetZoom(1.0f);
				s_ZoomSpeed = 0.0f;
			}

			if (camera.GetZoom() > 32.0f)
			{
				camera.SetZoom(32.0f);
				s_ZoomSpeed = 0.0f;
			}

			// Updates the projection matrix if needed
			// View matrix is handled by the transform component
			camera.PollUpdate();
		}
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		if (m_CameraEntity)
		{
			m_CameraEntity->GetComponent<OrthographicCameraComponent>().Camera.SetAspectRatio(width / height);
		}
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(NT_ATTACH_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(NT_ATTACH_EVENT_FN(OrthographicCameraController::OnWindowResized));
		dispatcher.Dispatch<MouseMovedEvent>(NT_ATTACH_EVENT_FN(OrthographicCameraController::OnMouseMoved));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		s_ZoomSpeed += e.GetYOffset() * m_MouseScrollSensitivity;
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

	bool OrthographicCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		static glm::vec2 lastMousePos = { e.GetX(), e.GetY() };

		if (Application::Get().GetWindow().IsSelected())
		{
			if (m_CameraEntity)
			{
				auto& transform = m_CameraEntity->GetComponent<TransformComponent>();
				auto& camera = m_CameraEntity->GetComponent<OrthographicCameraComponent>().Camera;

				// TODO: 
			}
		}

		lastMousePos = { e.GetX(), e.GetY() };
		return false;
	}
}
