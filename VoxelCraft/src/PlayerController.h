#pragma once

#include <Quark.h>

#include "Player.h"

class PlayerController : public Quark::PerspectiveCameraController
{
public:
	PlayerController(Player& player);

	void OnUpdate(float elapsedTime) override;
	void OnEvent(Quark::Event& e) override;

private:
	bool OnMouseMoved(Quark::MouseMovedEvent& e);
	bool OnWindowResized(Quark::WindowResizedEvent& e);

	Player& m_Player;
};
