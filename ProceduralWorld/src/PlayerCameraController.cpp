#include "PlayerCameraController.h"

static float s_ZoomSpeed = 0.0f;

void PlayerCameraController::OnEvent(Entropy::Event& e)
{
	Entropy::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Entropy::MouseScrolledEvent>(NT_ATTACH_EVENT_FN(PlayerCameraController::OnMouseScrolled));
}

void PlayerCameraController::OnUpdate(float elapsedTime)
{
	auto& camera = m_CameraEntity->GetComponent<Entropy::OrthographicCameraComponent>().Camera;

	auto& cameraTransform = m_CameraEntity->GetComponent<Entropy::Transform3DComponent>();
	auto& playerTransform = m_Player.GetComponent<Entropy::Transform3DComponent>();
	auto& playerPhysics   = m_Player.GetComponent<Entropy::PhysicsComponent>();

	// Distance between player and camera
	glm::vec3 distance = playerTransform.Position - cameraTransform.Position;
	cameraTransform.Position += distance * 8.0f * elapsedTime;

	// Zooming
	constexpr float zoomFrictionCoeff = 8.0f;
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

void PlayerCameraController::OnUpdate(float elapsedTime, Entropy::Entity player)
{
	m_Player = player;

	OnUpdate(elapsedTime);
}

bool PlayerCameraController::OnMouseScrolled(Entropy::MouseScrolledEvent& e)
{
	s_ZoomSpeed += e.GetYOffset() * m_MouseScrollSensitivity;
	return false;
}
