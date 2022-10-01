#include "qkpch.h"
#include "CameraController.h"

namespace Quark {

	static constexpr Float s_MovementSpeed = 12.0f;
	static constexpr Float s_MouseSensitivity = 0.002f;
	static constexpr Float s_MouseScrollSensitivity = 1.0f;
	static constexpr Float s_RollSensitivity = 12.0f;
	static constexpr Float s_RollFrictionCoeff = 8.0f;

	static constexpr Float s_MinFov = 1.0f;
	static constexpr Float s_MaxFov = 90.0f;
	static constexpr Float s_ZoomFrictionCoeff = 8.0f;

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ATTACH_EVENT_FN(OnMouseScrolled));
		dispatcher.Dispatch<MouseMovedEvent>(ATTACH_EVENT_FN(OnMouseMoved));
	}

	void CameraController::OnUpdate(Timestep elapsedTime)
	{
		auto& transform = GetComponent<Transform3DComponent>();
		auto& physics = GetComponent<PhysicsComponent>();

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
			physics.Velocity += transform.GetFrontVector() * elapsedTime.Seconds() * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::S))
		{
			physics.Velocity -= transform.GetFrontVector() * elapsedTime.Seconds() * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::D))
		{
			physics.Velocity += transform.GetRightVector() * elapsedTime.Seconds() * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::A))
		{
			physics.Velocity -= transform.GetRightVector() * elapsedTime.Seconds() * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::Space))
		{
			physics.Velocity += transform.GetTopVector() * elapsedTime.Seconds() * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::LeftShift))
		{
			physics.Velocity -= transform.GetTopVector() * elapsedTime.Seconds() * m_MovementSpeed;
		}

		if (Input::IsKeyPressed(KeyCode::Q))
		{
			m_AugularMomentum -= s_RollSensitivity * elapsedTime;
		}

		if (Input::IsKeyPressed(KeyCode::E))
		{
			m_AugularMomentum += s_RollSensitivity * elapsedTime;
		}

		m_AugularMomentum -= (m_AugularMomentum * s_RollFrictionCoeff) * elapsedTime;
		transform.Rotate(m_AugularMomentum * elapsedTime, transform.GetFrontVector());

		if (Input::IsKeyPressed(KeyCode::D0))
		{
			physics.Velocity = { 0.0f, 0.0f, 0.0f };
			transform.Position = { 0.0f, 0.0f, 0.0f };
			transform.Orientation = glm::angleAxis<Float>(0.0, Vec3(0.0, 0.0, 1.0));
			QK_CORE_TRACE("Teleported to world origin");
		}

		auto& camera = GetComponent<CameraComponent>().Camera;

		// Zooming
		m_ZoomSpeed -= (m_ZoomSpeed * s_ZoomFrictionCoeff) * elapsedTime;

		// Check if the fov needs to be changed
		if (glm::abs(m_ZoomSpeed) > glm::epsilon<Float>())
		{
			Float fov = camera.GetFov() - m_ZoomSpeed * elapsedTime.Seconds() * camera.GetFov();
			camera.SetFov((float)fov);
		}

		if (camera.GetFov() < s_MinFov)
		{
			camera.SetFov(s_MinFov);
			m_ZoomSpeed = 0.0;
		}
		else if (camera.GetFov() > s_MaxFov)
		{
			camera.SetFov(s_MaxFov);
			m_ZoomSpeed = 0.0;
		}
	}

	void CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomSpeed += e.GetYOffset() * s_MouseScrollSensitivity;
	}

	void CameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		auto& transform = GetComponent<Transform3DComponent>();
		auto& camera = GetComponent<CameraComponent>().Camera;

		Float fov = camera.GetFov() / 90.0f;
		Vec2f mouseMove = { e.GetXOffset(), e.GetYOffset() };
		Quatf qYaw      = glm::angleAxis<Float>(-mouseMove.x * s_MouseSensitivity * fov, Vec3(0.0, 1.0, 0.0) * (Quat)transform.Orientation);
		Quatf qPitch    = glm::angleAxis<Float>(-mouseMove.y * s_MouseSensitivity * fov, Vec3(1.0, 0.0, 0.0) * (Quat)transform.Orientation);

		transform.Rotate(qPitch * qYaw);
	}
}
