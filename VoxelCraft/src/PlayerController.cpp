#include "PlayerController.h"

PlayerController::PlayerController(Entropy::Entity camera)
{
	m_CameraEntity = camera;

	auto& physics = m_CameraEntity.GetComponent<Entropy::PhysicsComponent>();
	physics.Friction = 6.0f;
}

void PlayerController::OnUpdate(float elapsedTime)
{
	if (m_CameraEntity)
	{
		auto& transform = m_CameraEntity.GetComponent<Entropy::Transform3DComponent>();
		auto& physics = m_CameraEntity.GetComponent<Entropy::PhysicsComponent>();
		auto& camera = m_CameraEntity.GetComponent<Entropy::PerspectiveCameraComponent>().Camera;

		// Movement
		static const float defaultMovementSpeed = m_MovementSpeed;

		// Boost key
		if (Entropy::Input::IsKeyPressed(Entropy::Key::LeftControl))
		{
			m_MovementSpeed = 100.0f;
		}
		else
		{
			m_MovementSpeed = defaultMovementSpeed;
		}

		// Controls
		if (Entropy::Input::IsKeyPressed(Entropy::Key::W))
		{
			glm::vec3 front = { transform.GetFrontVector().x, 0.0f, transform.GetFrontVector().z };
			physics.Velocity += front * elapsedTime * m_MovementSpeed;
		}

		if (Entropy::Input::IsKeyPressed(Entropy::Key::S))
		{
			glm::vec3 front = { transform.GetFrontVector().x, 0.0f, transform.GetFrontVector().z };
			physics.Velocity -= front * elapsedTime * m_MovementSpeed;
		}

		if (Entropy::Input::IsKeyPressed(Entropy::Key::D))
		{
			glm::vec3 right = { transform.GetRightVector().x, 0.0f, transform.GetRightVector().z };
			physics.Velocity += transform.GetRightVector() * elapsedTime * m_MovementSpeed;
		}

		if (Entropy::Input::IsKeyPressed(Entropy::Key::A))
		{
			glm::vec3 right = { transform.GetRightVector().x, 0.0f, transform.GetRightVector().z };
			physics.Velocity -= right * elapsedTime * m_MovementSpeed;
		}

		if (Entropy::Input::IsKeyPressed(Entropy::Key::Space))
		{
			physics.Velocity.y += elapsedTime * m_MovementSpeed;
		}

		if (Entropy::Input::IsKeyPressed(Entropy::Key::LeftShift))
		{
			physics.Velocity.y -= elapsedTime * m_MovementSpeed;
		}

		camera.OnUpdate();
	}
}

void PlayerController::OnEvent(Entropy::Event& e)
{
	Entropy::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Entropy::WindowResizedEvent>(NT_ATTACH_EVENT_FN(PlayerController::OnWindowResized));
	dispatcher.Dispatch<Entropy::MouseMovedEvent>(NT_ATTACH_EVENT_FN(PlayerController::OnMouseMoved));
}

bool PlayerController::OnMouseMoved(Entropy::MouseMovedEvent& e)
{
	static glm::vec2 lastMousePos = { e.GetX(), e.GetY() };

	if (Entropy::Application::Get().GetWindow().IsSelected())
	{
		if (m_CameraEntity)
		{
			auto& transform = m_CameraEntity.GetComponent<Entropy::Transform3DComponent>();
			auto& camera = m_CameraEntity.GetComponent<Entropy::PerspectiveCameraComponent>().Camera;

			glm::vec2 mouseMove = { e.GetX() - lastMousePos.x, e.GetY() - lastMousePos.y };

			transform.Rotate(-mouseMove.x * m_MouseSensitivity, { 0.0f, 1.0f, 0.0f });
			transform.Rotate(-mouseMove.y * m_MouseSensitivity, transform.GetRightVector());
		}
	}

	lastMousePos = { e.GetX(), e.GetY() };
	return false;
}

bool PlayerController::OnWindowResized(Entropy::WindowResizedEvent& e)
{
	return false;
}
