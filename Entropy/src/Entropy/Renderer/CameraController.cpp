#include "CameraController.h"

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

	CameraController::CameraController(Entity& camera)
	{
		m_CameraEntity = &camera;
	}

	void CameraController::OnUpdate(float elapsedTime)
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity->GetComponent<TransformComponent>();
			auto& physics = m_CameraEntity->GetComponent<PhysicsComponent>();
			Camera& camera = m_CameraEntity->GetComponent<CameraComponent>();

			// Movement
			static const float defaultMovementSpeed = m_MovementSpeed;
			static const float defaultRollSensitivity = m_RollSensitivity;

			// Boost key
			if (Input::IsKeyPressed(Key::LeftControl))
			{
				m_MovementSpeed = 100.0f;
				m_RollSensitivity = 5.0f;
			}
			else
			{
				m_MovementSpeed = defaultMovementSpeed;
				m_RollSensitivity = defaultRollSensitivity;
			}

			// Controls
			if (Input::IsKeyPressed(Key::W))
			{
				physics.Velocity += glm::vec3(0.0f, 0.0f, 1.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::S))
			{
				physics.Velocity += glm::vec3(0.0f, 0.0f, -1.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::D))
			{
				physics.Velocity += glm::vec3(1.0f, 0.0f, 0.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::A))
			{
				physics.Velocity += glm::vec3(-1.0f, 0.0f, 0.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::Space))
			{
				physics.Velocity += glm::vec3(0.0f, 1.0f, 0.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::LeftShift))
			{
				physics.Velocity += glm::vec3(0.0f, -1.0f, 0.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::Q))
			{
				transform.Rotate(elapsedTime * m_RollSensitivity, glm::vec3(0.0f, 0.0f, -1.0f) * transform.Orientation);
			}

			if (Input::IsKeyPressed(Key::E))
			{
				transform.Rotate(elapsedTime * m_RollSensitivity, glm::vec3(0.0f, 0.0f, 1.0f) * transform.Orientation);
			}

			if (Input::IsKeyPressed(Key::D0))
			{
				physics.Velocity = { 0.0f, 0.0f, 0.0f };
				transform.Position = { 0.0f, 0.0f, 0.0f };
				transform.Orientation = glm::quat();
				NT_TRACE("Teleported to world origin");
			}

			// Zooming
			constexpr float zoomFrictionCoeff = 8.0f;
			s_ZoomSpeed -= (s_ZoomSpeed * zoomFrictionCoeff) * elapsedTime;

			// Check if the fov needs to be changed
			if (abs(s_ZoomSpeed) > 0.1f)
			{
				camera.SetFov(camera.GetFov() - s_ZoomSpeed * elapsedTime * m_MouseScrollSensitivity * camera.GetFov() / 120.0f);
			}

			if (camera.GetFov() < 1.0f)
			{
				camera.SetFov(1.0f);
				s_ZoomSpeed = 0.0f;
			}

			if (camera.GetFov() > 90.0f)
			{
				camera.SetFov(90.0f);
				s_ZoomSpeed = 0.0f;
			}

			// Updates the projection matrix if needed
			// View matrix is handled by the transform component
			camera.PollUpdate();
		}
	}

	void CameraController::OnResize(float aspectRatio)
	{
		if (m_CameraEntity)
		{
			m_CameraEntity->GetComponent<CameraComponent>().Camera.SetAspectRatio(aspectRatio);
		}
	}

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(NT_ATTACH_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(NT_ATTACH_EVENT_FN(CameraController::OnWindowResized));
		dispatcher.Dispatch<MouseMovedEvent>(NT_ATTACH_EVENT_FN(CameraController::OnMouseMoved));
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		s_ZoomSpeed += e.GetYOffset() * m_MouseScrollSensitivity;
		return false;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& e)
	{
		OnResize((float)e.GetWidth() / (float)e.GetHeight());
		return false;
	}

	bool CameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		static glm::vec2 lastMousePos = { e.GetX(), e.GetY() };

		if (Application::Get().GetWindow().IsSelected())
		{
			if (m_CameraEntity)
			{
				auto& transform = m_CameraEntity->GetComponent<TransformComponent>();
				Camera& camera = m_CameraEntity->GetComponent<CameraComponent>();

				glm::vec2 mouseMove = { e.GetX() - lastMousePos.x, e.GetY() - lastMousePos.y };
				glm::quat qYaw = glm::angleAxis(-mouseMove.x * m_MouseSensitivity * camera.GetFov() / 90.0f, glm::vec3(0.0f, 1.0f, 0.0f) * transform.Orientation);
				glm::quat qPitch = glm::angleAxis(-mouseMove.y * m_MouseSensitivity * camera.GetFov() / 90.0f, glm::vec3(1.0f, 0.0f, 0.0f) * transform.Orientation);

				transform.Rotate(qPitch * qYaw);
			}
		}

		lastMousePos = { e.GetX(), e.GetY() };
		return false;
	}
}
