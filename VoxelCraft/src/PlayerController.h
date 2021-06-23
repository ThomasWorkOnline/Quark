#pragma once

#include <Quark.h>

#include "Player.h"

enum class PlayerState
{
	Walking = 0,
	Flying
};

class PlayerController : public Quark::PerspectiveCameraController
{
public:
	PlayerController(Player& player);

	void OnUpdate(float elapsedTime) override;
	void OnEvent(Quark::Event& e) override;

	void SetPlayerState(PlayerState state) { m_PlayerState = state; }

private:
	bool OnMouseMoved(Quark::MouseMovedEvent& e);
	bool OnWindowResized(Quark::WindowResizedEvent& e);

	PlayerState m_PlayerState = PlayerState::Walking;

	Player& m_Player;
};
