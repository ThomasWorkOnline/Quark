#pragma once

#include <Quark.h>

class PlayerController : public Quark::PerspectiveCameraController
{
public:
	PlayerController() = default;
	PlayerController(Quark::Entity camera);

	void OnUpdate(float elapsedTime) override;
	void OnEvent(Quark::Event& e) override;

private:
	bool OnMouseMoved(Quark::MouseMovedEvent& e);
	bool OnWindowResized(Quark::WindowResizedEvent& e);

	float m_MovementSpeed = 100.0f;
	float m_MouseSensitivity = 0.002f;
};
