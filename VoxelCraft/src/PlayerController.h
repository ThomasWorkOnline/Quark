#pragma once

#include <Entropy.h>

class PlayerController : public Entropy::PerspectiveCameraController
{
public:
	PlayerController() = default;
	PlayerController(Entropy::Entity camera);

	void OnUpdate(float elapsedTime) override;
	void OnEvent(Entropy::Event& e) override;

private:
	bool OnMouseMoved(Entropy::MouseMovedEvent& e);
	bool OnWindowResized(Entropy::WindowResizedEvent& e);

	float m_MovementSpeed = 40.0f;
	float m_MouseSensitivity = 0.002f;
};
