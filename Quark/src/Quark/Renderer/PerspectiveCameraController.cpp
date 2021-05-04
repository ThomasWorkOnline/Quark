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

	static float s_ZoomSpeed = 0.0f;

	PerspectiveCameraController::PerspectiveCameraController(Entity& camera)
		: m_CameraEntity(camera) { }

	void PerspectiveCameraController::OnUpdate(float elapsedTime)
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity.GetComponent<Transform3DComponent>();
			auto& physics = m_CameraEntity.GetComponent<PhysicsComponent>();
			auto& camera = m_CameraEntity.GetComponent<PerspectiveCameraComponent>().Camera;

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
			if (Input::IsKeyPressed(Key::W))
			{
				physics.Velocity += transform.GetFrontVector() * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::S))
			{
				physics.Velocity -= transform.GetFrontVector() * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::D))
			{
				physics.Velocity += transform.GetRightVector() * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::A))
			{
				physics.Velocity -= transform.GetRightVector() * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::Space))
			{
				physics.Velocity += transform.GetTopVector() * elapsedTime * m_MovementSpeed;
			}

			if (Input::IsKeyPressed(Key::LeftShift))
			{
				physics.Velocity -= transform.GetTopVector() * elapsedTime * m_MovementSpeed;
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
			constexpr float zoomFrictionCoeff = 8.0f;
			s_ZoomSpeed -= (s_ZoomSpeed * zoomFrictionCoeff) * elapsedTime;

			// Check if the fov needs to be changed
			if (abs(s_ZoomSpeed) > 0.1f)
			{
				camera.SetFov(camera.GetFov() - s_ZoomSpeed * elapsedTime * camera.GetFov());
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

			camera.OnUpdate();
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
		s_ZoomSpeed += e.GetYOffset() * m_MouseScrollSensitivity;
		return false;
	}

	bool PerspectiveCameraController::OnWindowResized(WindowResizedEvent& e)
	{
		if (m_CameraEntity)
		{
			m_CameraEntity.GetComponent<PerspectiveCameraComponent>().Camera.SetAspectRatio((float)e.GetWidth() / (float)e.GetHeight());
		}
		return false;
	}

	bool PerspectiveCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		static glm::vec2 lastMousePos = { e.GetX(), e.GetY() };

		if (Application::Get().GetWindow().IsSelected())
		{
			if (m_CameraEntity)
			{
				auto& transform = m_CameraEntity.GetComponent<Transform3DComponent>();
				auto& camera = m_CameraEntity.GetComponent<PerspectiveCameraComponent>().Camera;

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
