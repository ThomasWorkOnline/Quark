#pragma once

#include <Quark.h>

#include "../Entity/Player.h"

namespace VoxelCraft {

	enum class PlayerState
	{
		Walking = 0,
		Flying
	};

	class PlayerController
	{
	public:
		PlayerController(Player& player);

		void OnUpdate(float elapsedTime);
		void OnEvent(Quark::Event& e);

		void SetPlayerState(PlayerState state) { m_PlayerState = state; }

	private:
		bool OnMouseMoved(Quark::MouseMovedEvent& e);
		bool OnWindowResized(Quark::WindowResizedEvent& e);

		std::atomic<PlayerState> m_PlayerState = PlayerState::Walking;

		Player& m_Player;

		float m_MovementSpeed = 0.0f;
	};
}
