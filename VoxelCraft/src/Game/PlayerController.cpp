#include "PlayerController.h"

#include "../Components/GravityComponent.h"

namespace VoxelCraft {

	static constexpr float s_AirFriction = 1.0f;
	static constexpr float s_PlayerAcceleration = 15.0f;
	static constexpr float s_PlayerMidAirAcceleration = 6.0f;

	static constexpr Quark::Key s_SprintKey		= Quark::Key::LeftControl;
	static constexpr Quark::Key s_SneakKey		= Quark::Key::LeftShift;
	static constexpr Quark::Key s_JumpKey		= Quark::Key::Space;
	static constexpr Quark::Key s_ForwardKey	= Quark::Key::W;
	static constexpr Quark::Key s_LeftKey		= Quark::Key::A;
	static constexpr Quark::Key s_BackwardKey	= Quark::Key::S;
	static constexpr Quark::Key s_RightKey		= Quark::Key::D;

	static struct ControlKeys
	{
		ControlKeys()
			: W(false), A(false), S(false), D(false) {}

		ControlKeys& operator= (const ControlKeys& other)
		{
			W = other.W; A = other.A; S = other.S; D = other.D;
			return *this;
		}

		bool Walking() const { return W || A || S || D; }

		bool W, A, S, D;
	};

	PlayerController::PlayerController(Player& player)
		: m_Player(player) {}

	void PlayerController::OnUpdate(float elapsedTime)
	{
		if (m_Player)
		{
			auto& transform = m_Player.GetComponent<Quark::Transform3DComponent>();
			auto& physics = m_Player.GetComponent<Quark::PhysicsComponent>();
			auto& camera = m_Player.GetComponent<Quark::PerspectiveCameraComponent>().Camera;
			auto& gravity = m_Player.GetComponent<GravityComponent>();

			glm::vec3 front = glm::normalize(glm::vec3(transform.GetFrontVector().x, 0.0f, transform.GetFrontVector().z));
			glm::vec3 right = glm::normalize(glm::vec3(transform.GetRightVector().x, 0.0f, transform.GetRightVector().z));

			bool onGround = m_Player.IsTouchingGround();
			bool sprinting = Quark::Input::IsKeyPressed(s_SprintKey);

			ControlKeys keys;
			static ControlKeys lastKeys;

			keys.W = Quark::Input::IsKeyPressed(s_ForwardKey);
			keys.A = Quark::Input::IsKeyPressed(s_LeftKey);
			keys.S = Quark::Input::IsKeyPressed(s_BackwardKey);
			keys.D = Quark::Input::IsKeyPressed(s_RightKey);

			switch (m_PlayerState)
			{
			case PlayerState::Walking:

				/// //////////////////////////////////////////////////////
				/// WALKING
				///

				// Jump
				if (Quark::Input::IsKeyPressed(s_JumpKey))
				{
					if (m_Player.IsTouchingGround())
					{
						physics.Velocity.y = 10.0f;
					}
				}

				{
					// Controls
					static float forwardSpeed = 0.f;
					static float backwardSpeed = 0.f;
					static float rightSpeed = 0.f;
					static float leftSpeed = 0.f;

					if (keys.W)
					{
						if (onGround)
						{
							// Boost key
							if (sprinting)
							{
								m_MovementSpeed = m_Player.GetSettings().SprintMovementSpeed;
							}
							else
							{
								m_MovementSpeed = m_Player.GetSettings().BaseMovementSpeed;
							}

							forwardSpeed += s_PlayerAcceleration * elapsedTime;

							if (forwardSpeed > m_MovementSpeed)
								forwardSpeed = m_MovementSpeed;
						}
						else
						{
							forwardSpeed += s_PlayerMidAirAcceleration * elapsedTime;
						}
					}
					else
					{
						if (onGround)
						{
							forwardSpeed -= forwardSpeed * s_PlayerAcceleration * elapsedTime;
						}
					}

					if (keys.S)
					{
						if (onGround)
						{
							// Boost key
							if (sprinting)
							{
								m_MovementSpeed = m_Player.GetSettings().SprintMovementSpeed;
							}
							else
							{
								m_MovementSpeed = m_Player.GetSettings().BaseMovementSpeed;
							}

							backwardSpeed += s_PlayerAcceleration * elapsedTime;

							if (backwardSpeed > m_MovementSpeed)
								backwardSpeed = m_MovementSpeed;
						}
						else
						{
							backwardSpeed += s_PlayerMidAirAcceleration * elapsedTime;
						}
					}
					else
					{
						if (onGround)
						{
							backwardSpeed -= backwardSpeed * s_PlayerAcceleration * elapsedTime;
						}
					}

					if (keys.D)
					{
						if (onGround)
						{
							// Boost key
							if (sprinting)
							{
								m_MovementSpeed = m_Player.GetSettings().SprintMovementSpeed;
							}
							else
							{
								m_MovementSpeed = m_Player.GetSettings().BaseMovementSpeed;
							}

							rightSpeed += s_PlayerAcceleration * elapsedTime;

							if (rightSpeed > m_MovementSpeed)
								rightSpeed = m_MovementSpeed;
						}
						else
						{
							rightSpeed += s_PlayerMidAirAcceleration * elapsedTime;
						}
					}
					else
					{
						if (onGround)
						{
							rightSpeed -= rightSpeed * s_PlayerAcceleration * elapsedTime;
						}
					}

					if (keys.A)
					{
						if (onGround)
						{
							// Boost key
							if (sprinting)
							{
								m_MovementSpeed = m_Player.GetSettings().SprintMovementSpeed;
							}
							else
							{
								m_MovementSpeed = m_Player.GetSettings().BaseMovementSpeed;
							}

							leftSpeed += s_PlayerAcceleration * elapsedTime;

							if (leftSpeed > m_MovementSpeed)
								leftSpeed = m_MovementSpeed;
						}
						else
						{
							leftSpeed += s_PlayerMidAirAcceleration * elapsedTime;
						}
					}
					else
					{
						if (onGround)
						{
							leftSpeed -= leftSpeed * s_PlayerAcceleration * elapsedTime;
						}
					}

					transform.Position += forwardSpeed * front * elapsedTime;
					transform.Position += backwardSpeed * -front * elapsedTime;
					transform.Position += rightSpeed * right * elapsedTime;
					transform.Position += leftSpeed * -right * elapsedTime;
				}

				// Air friction
				physics.Velocity -= physics.Velocity * s_AirFriction * elapsedTime;

				// Gravity
				if (gravity.Affected && !m_Player.IsTouchingGround())
				{
					physics.Velocity.y -= gravity.GravityConstant * elapsedTime;
				}
				break;

			case PlayerState::Flying:

				/// //////////////////////////////////////////////////////
				/// FLYING
				///

				// Boost key
				if (Quark::Input::IsKeyPressed(s_SprintKey))
				{
					m_MovementSpeed = m_Player.GetSettings().BoostFlyingMovementSpeed;
				}
				else
				{
					m_MovementSpeed = m_Player.GetSettings().FlyingMovementSpeed;
				}

				{
					// Controls
					if (Quark::Input::IsKeyPressed(s_ForwardKey))
					{
						physics.Velocity += front * m_MovementSpeed * elapsedTime;
					}

					if (Quark::Input::IsKeyPressed(s_BackwardKey))
					{
						physics.Velocity += -front * m_MovementSpeed * elapsedTime;
					}

					if (Quark::Input::IsKeyPressed(s_RightKey))
					{
						physics.Velocity += right * m_MovementSpeed * elapsedTime;
					}

					if (Quark::Input::IsKeyPressed(s_LeftKey))
					{
						physics.Velocity += -right * m_MovementSpeed * elapsedTime;
					}
				}

				// Up / down
				if (Quark::Input::IsKeyPressed(s_JumpKey))
				{
					physics.Velocity.y += elapsedTime * m_MovementSpeed;
				}

				if (Quark::Input::IsKeyPressed(s_SneakKey))
				{
					physics.Velocity.y -= elapsedTime * m_MovementSpeed;
				}
				break;
			}

			lastKeys = keys;

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
			if (m_Player)
			{
				auto& transform = m_Player.GetComponent<Quark::Transform3DComponent>();
				auto& camera = m_Player.GetComponent<Quark::PerspectiveCameraComponent>().Camera;

				static glm::quat qPitch = glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
				static glm::quat qYaw = glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

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
		if (m_Player)
		{
			auto& camera = m_Player.GetComponent<Quark::PerspectiveCameraComponent>().Camera;
			camera.SetAspectRatio((float)e.GetWidth() / (float)e.GetHeight());
		}
		return false;
	}
}
