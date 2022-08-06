#include "qkpch.h"
#include "CameraController.h"

namespace Quark {

	static constexpr float s_RollSensitivity = 1.0f;
	static constexpr float s_MovementSpeed = 12.0f;
	static constexpr float s_MouseSensitivity = 0.002f;
	static constexpr float s_MouseScrollSensitivity = 1.0f;

	static constexpr float s_ZoomFrictionCoeff = 8.0f;

	void CameraController::OnUpdate(Timestep elapsedTime)
	{
		auto& transform = GetComponent<Transform3DComponent>();
		auto& physics = GetComponent<PhysicsComponent>();
		auto& camera = GetComponent<CameraComponent>().Camera;

		// Boost Key
		if (Input::IsKeyPressed(KeyCode::LeftControl))
		{
			m_MovementSpeed = 100.0f;
			m_RollSensitivity = 5.0f;
		}
		else
		{
			m_MovementSpeed = s_MovementSpeed;
			m_RollSensitivity = s_MouseScrollSensitivity;
		}

		// Controls
		if (Input::IsKeyPressed(KeyCode::W))
		{
			physics.Velocity += transform.GetFrontVector() * (Float)(elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::S))
		{
			physics.Velocity -= transform.GetFrontVector() * (Float)(elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::D))
		{
			physics.Velocity += transform.GetRightVector() * (Float)(elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::A))
		{
			physics.Velocity -= transform.GetRightVector() * (Float)(elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::Space))
		{
			physics.Velocity += transform.GetTopVector() * (Float)(elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::LeftShift))
		{
			physics.Velocity -= transform.GetTopVector() * (Float)(elapsedTime * m_MovementSpeed);
		}

		if (Input::IsKeyPressed(KeyCode::Q))
		{
			transform.Rotate(elapsedTime * s_RollSensitivity, -transform.GetFrontVector());
		}

		if (Input::IsKeyPressed(KeyCode::E))
		{
			transform.Rotate(elapsedTime * s_RollSensitivity, transform.GetFrontVector());
		}

		if (Input::IsKeyPressed(KeyCode::D0))
		{
			physics.Velocity = { 0.0f, 0.0f, 0.0f };
			transform.Position = { 0.0f, 0.0f, 0.0f };
			transform.Orientation = glm::angleAxis<Float>(0.0f, Vec3f(0.0f, 0.0f, 1.0f));
			QK_CORE_TRACE("Teleported to world origin");
		}

		// Zooming
		m_ZoomSpeed -= (m_ZoomSpeed * s_ZoomFrictionCoeff) * elapsedTime;

		// Check if the fov needs to be changed
		if (std::abs(m_ZoomSpeed) > glm::epsilon<Float>())
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

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ATTACH_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<MouseMovedEvent>(ATTACH_EVENT_FN(CameraController::OnMouseMoved));
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomSpeed += e.GetYOffset() * s_MouseScrollSensitivity;
		return false;
	}

	bool CameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		auto& transform = GetComponent<Transform3DComponent>();
		auto& camera = GetComponent<CameraComponent>().Camera;

		Vec2f mouseMove = { e.GetXOffset(), e.GetYOffset() };
		Quatf qYaw      = glm::angleAxis(-mouseMove.x * s_MouseSensitivity * camera.GetFov() / 90.0f, Vec3(0.0f, 1.0f, 0.0f) * transform.Orientation);
		Quatf qPitch    = glm::angleAxis(-mouseMove.y * s_MouseSensitivity * camera.GetFov() / 90.0f, Vec3(1.0f, 0.0f, 0.0f) * transform.Orientation);

		transform.Rotate(qPitch * qYaw);
		return false;
	}
}
