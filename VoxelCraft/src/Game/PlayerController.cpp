#include "PlayerController.h"

#include "../Components/GravityComponent.h"

namespace VoxelCraft {

	static constexpr float s_AirFriction = 1.0f;
	static constexpr float s_GroundFriction = 10.0f;

	static glm::vec3 s_ForwardAcceleration;
	static glm::vec3 s_ForwardVelocity;

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

			switch (m_PlayerState)
			{
			case PlayerState::Walking:

				/// //////////////////////////////////////////////////////
				/// WALKING
				///

				// Boost key
				if (Quark::Input::IsKeyPressed(Quark::Key::LeftShift))
				{
					m_MovementSpeed = m_Player.GetSettings().SprintMovementSpeed;
				}
				else
				{
					m_MovementSpeed = m_Player.GetSettings().BaseMovementSpeed;
				}

				// Jump
				if (Quark::Input::IsKeyPressed(Quark::Key::Space))
				{
					if (m_Player.IsTouchingGround())
					{
						physics.Velocity.y = 10.0f;
					}
				}

				{
					// Controls
					ControlKeys keys;
					static ControlKeys lastKeys;

					keys.W = Quark::Input::IsKeyPressed(Quark::Key::W);
					keys.A = Quark::Input::IsKeyPressed(Quark::Key::A);
					keys.S = Quark::Input::IsKeyPressed(Quark::Key::S);
					keys.D = Quark::Input::IsKeyPressed(Quark::Key::D);

					glm::vec3 front = glm::normalize(glm::vec3(transform.GetFrontVector().x, 0.0f, transform.GetFrontVector().z));
					glm::vec3 right = glm::normalize(glm::vec3(transform.GetRightVector().x, 0.0f, transform.GetRightVector().z));

					if (keys.W)
					{
						s_ForwardAcceleration = front * m_MovementSpeed;
						s_ForwardVelocity = s_ForwardAcceleration * elapsedTime;

						physics.Velocity += s_ForwardVelocity;
					}
					else
					{
						if (lastKeys.W != keys.W)
						{
							s_ForwardAcceleration = glm::vec3(0.f);
							std::cout << "I stopped walking farwards.\n";
						}

						if (m_Player.IsTouchingGround())
						{
							physics.Velocity -= physics.Velocity * s_GroundFriction * elapsedTime;
						}
					}

					if (keys.S)
					{
						physics.Velocity += -front * m_MovementSpeed * elapsedTime;
					}
					else
					{
						if (lastKeys.S != keys.S)
						{
							std::cout << "I stopped walking backwards.\n";
						}
					}

					if (keys.D)
					{
						physics.Velocity += right * m_MovementSpeed * elapsedTime;
					}
					else
					{
						if (lastKeys.D != keys.D)
						{
							std::cout << "I stopped walking right.\n";
						}
					}

					if (keys.A)
					{
						physics.Velocity += -right * m_MovementSpeed * elapsedTime;
					}
					else
					{
						if (lastKeys.A != keys.A)
						{
							std::cout << "I stopped walking left.\n";
						}
					}

					// Air friction
					physics.Velocity -= physics.Velocity * s_AirFriction * elapsedTime;

					lastKeys = keys;
				}

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
				if (Quark::Input::IsKeyPressed(Quark::Key::LeftShift))
				{
					m_MovementSpeed = m_Player.GetSettings().BoostFlyingMovementSpeed;
				}
				else
				{
					m_MovementSpeed = m_Player.GetSettings().FlyingMovementSpeed;
				}

				{
					// Controls
					glm::vec3 front = glm::normalize(glm::vec3(transform.GetFrontVector().x, 0.0f, transform.GetFrontVector().z));
					glm::vec3 right = glm::normalize(glm::vec3(transform.GetRightVector().x, 0.0f, transform.GetRightVector().z));

					if (Quark::Input::IsKeyPressed(Quark::Key::W))
					{
						physics.Velocity += front * m_MovementSpeed * elapsedTime;
					}

					if (Quark::Input::IsKeyPressed(Quark::Key::S))
					{
						physics.Velocity += -front * m_MovementSpeed * elapsedTime;
					}

					if (Quark::Input::IsKeyPressed(Quark::Key::D))
					{
						physics.Velocity += right * m_MovementSpeed * elapsedTime;
					}

					if (Quark::Input::IsKeyPressed(Quark::Key::A))
					{
						physics.Velocity += -right * m_MovementSpeed * elapsedTime;
					}
				}

				// Up / down
				if (Quark::Input::IsKeyPressed(Quark::Key::Space))
				{
					physics.Velocity.y += elapsedTime * m_MovementSpeed;
				}

				if (Quark::Input::IsKeyPressed(Quark::Key::LeftControl))
				{
					physics.Velocity.y -= elapsedTime * m_MovementSpeed;
				}
				break;
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
