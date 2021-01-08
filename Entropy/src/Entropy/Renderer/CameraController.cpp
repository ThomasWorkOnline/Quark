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

	CameraController::CameraController(Entity camera)
		: m_CameraEntity(camera)
	{
	}

	void CameraController::OnUpdate(float elapsedTime)
	{
		static const float airFrictionCoeff = 4.0f;
		static float lastMouseX = 0.0f;
		static float lastMouseY = 0.0f;
		float mouseMoveX = Input::GetMouseX() - lastMouseX;
		float mouseMoveY = Input::GetMouseY() - lastMouseY;

		auto& transform = m_CameraEntity.GetComponent<TransformComponent>();
		auto& camera = m_CameraEntity.GetComponent<CameraComponent>().Camera;
		auto& physics = m_CameraEntity.GetComponent<PhysicsComponent>();

		// View
		if (Application::Get().GetWindow().IsSelected())
		{
			if (lastMouseX != Input::GetMouseX() || lastMouseY != Input::GetMouseY())
			{
				glm::quat qYaw = glm::angleAxis(-mouseMoveX * m_MouseSensitivity * camera.GetFov() / 120.0f, glm::vec3(0.0f, 1.0f, 0.0f) * transform.Orientation);
				glm::quat qPitch = glm::angleAxis(-mouseMoveY * m_MouseSensitivity * camera.GetFov() / 120.0f, glm::vec3(1.0f, 0.0f, 0.0f) * transform.Orientation);

				transform.Rotate(qPitch * qYaw);
			}
		}

		lastMouseX = Input::GetMouseX();
		lastMouseY = Input::GetMouseY();

		// Movement
		static const float defaultMovementSpeed = m_MovementSpeed;
		static const float defaultRollSensitivity = m_RollSensitivity;

		// Boost key
		if (Input::IsKeyPressed(KeyCode::LeftControl))
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
		if (Input::IsKeyPressed(KeyCode::W))
		{
			physics.Velocity += glm::vec3(0.0f, 0.0f, 1.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::S))
		{
			physics.Velocity += glm::vec3(0.0f, 0.0f, -1.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::D))
		{
			physics.Velocity += glm::vec3(1.0f, 0.0f, 0.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::A))
		{
			physics.Velocity += glm::vec3(-1.0f, 0.0f, 0.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::Space))
		{
			physics.Velocity += glm::vec3(0.0f, 1.0f, 0.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::LeftShift))
		{
			physics.Velocity += glm::vec3(0.0f, -1.0f, 0.0f) * transform.Orientation * elapsedTime * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::Q))
		{
			transform.Rotate(elapsedTime * m_RollSensitivity, glm::vec3(0.0f, 0.0f, -1.0f) * transform.Orientation);
		}

		if (Input::IsKeyPressed(KeyCode::E))
		{
			transform.Rotate(elapsedTime * m_RollSensitivity, glm::vec3(0.0f, 0.0f, 1.0f) * transform.Orientation);
		}

		if (Input::IsKeyPressed(KeyCode::D0))
		{
			//m_Camera.SetVelocity(glm::vec3());
			transform.Position = { 0.0f, 0.0f, 0.0f };
			transform.Orientation = glm::quat();
			NT_TRACE("Teleported to world origin");
		}

		// Zooming
		static const float zoomFrictionCoeff = 8.0f;
		s_ZoomSpeed -= s_ZoomSpeed * elapsedTime * zoomFrictionCoeff;

		// Check if the fov needs to be changed
		if (abs(s_ZoomSpeed) > 0.1f)
			camera.SetFov(camera.GetFov() - s_ZoomSpeed * elapsedTime * m_MouseScrollSensitivity * camera.GetFov() / 120.0f);

		if (camera.GetFov() < 1.0f)
		{
			camera.SetFov(1.0f);
			s_ZoomSpeed = 0.0f;
		}

		if (camera.GetFov() > 120.0f)
		{
			camera.SetFov(120.0f);
			s_ZoomSpeed = 0.0f;
		}

		camera.PollUpdate();
	}

	void CameraController::OnResize(float aspectRatio)
	{
		auto& camera = m_CameraEntity.GetComponent<CameraComponent>().Camera;
		camera.SetAspectRatio(aspectRatio);
	}

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(NT_ATTACH_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(NT_ATTACH_EVENT_FN(CameraController::OnWindowResized));
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
}
