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
	void SetGravityEnabled(bool enabled) { m_GravityEnabled = enabled; }

private:
	bool OnMouseMoved(Quark::MouseMovedEvent& e);
	bool OnWindowResized(Quark::WindowResizedEvent& e);

	std::atomic_bool m_GravityEnabled = false;
	std::atomic<PlayerState> m_PlayerState = PlayerState::Flying;

	Player& m_Player;
};
