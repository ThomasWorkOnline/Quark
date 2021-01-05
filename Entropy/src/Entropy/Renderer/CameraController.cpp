#include "CameraController.h"

#include "../Core/Core.h"
#include "../Core/Input.h"
#include "../Core/KeyCodes.h"
#include "../Core/Application.h"

#include "../Tools/EasingCurves.h"

namespace Entropy {

	static float s_ZoomSpeed = 0.0f;

	CameraController::CameraController(float aspectRatio)
		: m_Camera(aspectRatio)
	{
	}

	void CameraController::OnUpdate(float elapsedTime)
	{
		static const float airFrictionCoeff = 4.0f;
		static float lastMouseX = 0.0f;
		static float lastMouseY = 0.0f;
		float mouseMoveX = Input::GetMouseX() - lastMouseX;
		float mouseMoveY = Input::GetMouseY() - lastMouseY;

		// View
		if (Application::Get().GetWindow().IsSelected())
		{
			if (lastMouseX != Input::GetMouseX() || lastMouseY != Input::GetMouseY())
			{
				glm::quat qYaw = glm::angleAxis(-mouseMoveX * m_MouseSensitivity, glm::vec3(0.0f, 1.0f, 0.0f) * m_Camera.GetOrientation());
				glm::quat qPitch = glm::angleAxis(-mouseMoveY * m_MouseSensitivity, glm::vec3(1.0f, 0.0f, 0.0f) * m_Camera.GetOrientation());

				m_Camera.Rotate(qPitch * qYaw);
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
			m_Camera.SetVelocity(m_Camera.GetVelocity() + m_Camera.GetFowardVector() * elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::S))
		{
			m_Camera.SetVelocity(m_Camera.GetVelocity() + m_Camera.GetBackwardVector() * elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::D))
		{
			m_Camera.SetVelocity(m_Camera.GetVelocity() + m_Camera.GetRightVector() * elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::A))
		{
			m_Camera.SetVelocity(m_Camera.GetVelocity() + m_Camera.GetLeftVector() * elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::Space))
		{
			m_Camera.SetVelocity(m_Camera.GetVelocity() + m_Camera.GetTopVector() * elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::LeftShift))
		{
			m_Camera.SetVelocity(m_Camera.GetVelocity() + m_Camera.GetBottomVector() * elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::Q))
		{
			m_Camera.Rotate(elapsedTime * m_RollSensitivity, m_Camera.GetBackwardVector());
		}

		if (Input::IsKeyPressed(KeyCode::E))
		{
			m_Camera.Rotate(elapsedTime * m_RollSensitivity, m_Camera.GetFowardVector());
		}

		if (Input::IsKeyPressed(KeyCode::D0))
		{
			m_Camera.SetVelocity(glm::vec3());
			m_Camera.SetPosition(glm::vec3());
			m_Camera.SetOrientation(glm::quat());
			NT_TRACE("Teleported to world origin");
		}

		// Calculating friction loss
		m_Camera.SetVelocity(m_Camera.GetVelocity() - m_Camera.GetVelocity() * elapsedTime * airFrictionCoeff);

		// Clamping is important as the view matrix is recalculated every time the position changes
		float cameraSpeed = glm::length(m_Camera.GetVelocity());

		// Setting new position
		// Check if position needs to be changed
		if (cameraSpeed > 0.01f)
			m_Camera.SetPosition(m_Camera.GetPosition() + m_Camera.GetVelocity() * elapsedTime);

		// Zooming
		static const float zoomFrictionCoeff = 8.0f;
		s_ZoomSpeed -= s_ZoomSpeed * elapsedTime * zoomFrictionCoeff;

		// Check if the fov needs to be changed
		if (abs(s_ZoomSpeed) > 0.1f)
			m_Camera.SetFov(m_Camera.GetFov() - s_ZoomSpeed * elapsedTime * m_MouseScrollSensitivity * m_Camera.GetFov() / 120.0f);

		if (m_Camera.GetFov() < 1.0f)
		{
			m_Camera.SetFov(1.0f);
			s_ZoomSpeed = 0.0f;
		}

		if (m_Camera.GetFov() > 120.0f)
		{
			m_Camera.SetFov(120.0f);
			s_ZoomSpeed = 0.0f;
		}

		// Update camera matrices
		m_Camera.CheckAndUpdate();
	}

	void CameraController::OnResize(float aspectRatio)
	{
		m_Camera.SetProjectionMatrix(aspectRatio);
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
