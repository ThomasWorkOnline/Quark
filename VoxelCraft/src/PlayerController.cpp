#include "PlayerController.h"

PlayerController::PlayerController(Player& player)
	: m_Player(player)
{
	m_CameraEntity = player;

	auto& physics = m_CameraEntity.GetComponent<Quark::PhysicsComponent>();
	physics.Friction = 6.0f;
}

void PlayerController::OnUpdate(float elapsedTime)
{
	if (m_CameraEntity)
	{
		auto& transform = m_CameraEntity.GetComponent<Quark::Transform3DComponent>();
		auto& physics = m_CameraEntity.GetComponent<Quark::PhysicsComponent>();
		auto& camera = m_CameraEntity.GetComponent<Quark::PerspectiveCameraComponent>().Camera;

		// Boost key
		if (Quark::Input::IsKeyPressed(Quark::Key::LeftControl))
		{
			m_MovementSpeed = m_Player.GetSettings().SprintMovementSpeed;
		}
		else
		{
			m_MovementSpeed = m_Player.GetSettings().BaseMovementSpeed;
		}

		// Controls
		if (Quark::Input::IsKeyPressed(Quark::Key::W))
		{
			glm::vec3 front = { transform.GetFrontVector().x, 0.0f, transform.GetFrontVector().z };
			front = glm::normalize(front);
			physics.Velocity -= -front * elapsedTime * m_MovementSpeed;
		}

		if (Quark::Input::IsKeyPressed(Quark::Key::S))
		{
			glm::vec3 front = { transform.GetFrontVector().x, 0.0f, transform.GetFrontVector().z };
			front = glm::normalize(front);
			physics.Velocity -= front * elapsedTime * m_MovementSpeed;
		}

		if (Quark::Input::IsKeyPressed(Quark::Key::D))
		{
			glm::vec3 right = { transform.GetRightVector().x, 0.0f, transform.GetRightVector().z };
			right = glm::normalize(right);
			physics.Velocity -= -right * elapsedTime * m_MovementSpeed;
		}

		if (Quark::Input::IsKeyPressed(Quark::Key::A))
		{
			glm::vec3 right = { transform.GetRightVector().x, 0.0f, transform.GetRightVector().z };
			right = glm::normalize(right);
			physics.Velocity -= right * elapsedTime * m_MovementSpeed;
		}

		if (Quark::Input::IsKeyPressed(Quark::Key::Space))
		{
			physics.Velocity.y += elapsedTime * m_MovementSpeed;
		}

		if (Quark::Input::IsKeyPressed(Quark::Key::LeftShift))
		{
			physics.Velocity.y -= elapsedTime * m_MovementSpeed;
		}

		camera.OnUpdate();
	}
}

void PlayerController::OnEvent(Quark::Event& e)
{
	Quark::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Quark::WindowResizedEvent>(ATTACH_EVENT_FN(PlayerController::OnWindowResized));
	dispatcher.Dispatch<Quark::MouseMovedEvent>(ATTACH_EVENT_FN(PlayerController::OnMouseMoved));
}

bool PlayerController::OnMouseMoved(Quark::MouseMovedEvent& e)
{
	static glm::vec2 lastMousePos = { e.GetX(), e.GetY() };

	if (Quark::Application::Get().GetWindow().IsSelected())
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity.GetComponent<Quark::Transform3DComponent>();
			auto& camera = m_CameraEntity.GetComponent<Quark::PerspectiveCameraComponent>().Camera;

			static glm::quat qPitch = glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			static glm::quat qYaw	= glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

			glm::vec2 mouseMove = { e.GetX() - lastMousePos.x, e.GetY() - lastMousePos.y };
			glm::vec3 rightVector = glm::normalize(glm::vec3(transform.GetRightVector().x, 0.0f, transform.GetRightVector().z));

			glm::quat pitch = glm::angleAxis(-mouseMove.y * m_Player.GetSettings().MouseSensitivity, rightVector);
			glm::quat yaw = glm::angleAxis(-mouseMove.x * m_Player.GetSettings().MouseSensitivity, glm::vec3(0.0f, 1.0f, 0.0f));

			qPitch *= pitch;
			qYaw *= yaw;

			constexpr float threshold = glm::half_pi<float>() - 0.0001f;
			if (glm::angle(qPitch) > threshold && glm::axis(qPitch).x < 0)
			{
				qPitch = glm::angleAxis(threshold, glm::axis(qPitch));
			}

			if (glm::angle(qPitch) > threshold && glm::axis(qPitch).x > 0)
			{
				qPitch = glm::angleAxis(threshold, glm::axis(qPitch));
			}

			transform.Rotate(pitch * yaw);
		}
	}

	lastMousePos = { e.GetX(), e.GetY() };
	return false;
}

bool PlayerController::OnWindowResized(Quark::WindowResizedEvent& e)
{
	if (m_CameraEntity)
	{
		auto& camera = m_CameraEntity.GetComponent<Quark::PerspectiveCameraComponent>().Camera;
		camera.SetAspectRatio((float)e.GetWidth() / (float)e.GetHeight());
	}
	return false;
}
