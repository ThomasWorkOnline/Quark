#include <Quark.h>

#include "../Components/GravityComponent.h"
#include "../World/ChunkIdentifier.h"
#include "../World/World.h"
#include "../Rendering/Renderer.h"
#include "Resources.h"
#include "PlayerController.h"

namespace VoxelCraft {

	static RendererStats s_RenderStatsCache;
	static std::atomic_bool s_FlagPlayerGravity;

	class VoxelCraft : public Quark::Application
	{
	public:
		VoxelCraft()
		{
			QK_TIME_SCOPE_DEBUG(VoxelCraft::VoxelCraft);

			Quark::RenderCommand::SetClearColor(Quark::DecodeSRGB(glm::vec4(0.78f, 0.90f, 0.93f, 1.0f)));

			GetWindow().Select();
			GetWindow().SetVSync(false);
			GetWindow().SetFullScreen(false);

			Resources::Initialize();
			Renderer::Initialize();

			m_World->SetChunkLoadedCallback(ATTACH_EVENT_FN(VoxelCraft::OnWorldChunkLoaded));

			const Position3D& playerPos = m_Player.GetPosition();
			m_World->Loader.Coord = playerPos.ToChunkCoord();
			m_World->Loader.Start();
		}

		~VoxelCraft() override
		{
			Renderer::Shutdown();
		}

		void OnUpdate(float elapsedTime) override
		{

			UpdateLoader();

			m_World->OnUpdate(elapsedTime);
			ProcessPlayerCollision();

			m_Player.GetComponent<GravityComponent>().Affected = s_FlagPlayerGravity;

			// Controls
			m_Controller.OnUpdate(elapsedTime);

			// Rendering
			{
				Renderer::BeginScene(m_Player.GetComponent<Quark::PerspectiveCameraComponent>().Camera.GetProjection(), m_Player.GetCameraTransformNoPosition(), m_Player.GetHeadPosition());

				m_World->Map.Foreach([this](Chunk* data)
					{
						Renderer::SubmitChunk(data);
					});

				s_RenderStatsCache = Renderer::GetStats();

				Renderer::EndScene();
			}

			{
				const auto& window = Quark::Application::Get().GetWindow();
				Renderer::RenderUIScene(window.GetWidth(), window.GetHeight());
			}

			CalculateAppPerformance(elapsedTime);
		}

		void OnEvent(Quark::Event& e) override
		{
			Quark::EventDispatcher dispatcher(e);
			dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnKeyPressed));
			dispatcher.Dispatch<Quark::MouseButtonPressedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnMouseButtonPressed));
			dispatcher.Dispatch<Quark::WindowResizedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnWindowResized));

			if (!e.Handled)
			{
				m_Controller.OnEvent(e);
			}
		}

		bool OnKeyPressed(Quark::KeyPressedEvent& e)
		{
			static bool viewUnloadedChunks = false;

			switch (e.GetKeyCode())
			{
			case Quark::Key::Escape:
				GetWindow().Deselect();
				break;
			case Quark::Key::F11:
				GetWindow().SetFullScreen(!GetWindow().IsFullscreen());
				break;
			case Quark::Key::F1:
				Renderer::SwitchShader();
				break;
			case Quark::Key::F2:
				viewUnloadedChunks = !viewUnloadedChunks;
				Renderer::ViewUnloadedChunks(viewUnloadedChunks);
				break;
			case Quark::Key::T:
				std::cout << "====== WORLD SUMMARY ======\n";
				std::cout << m_World->Map.Count() << " chunks active\n";
				std::cout << "Idling: " << (m_World->Loader.Idling() ? "true" : "false") << '\n';
				std::cout << "Draw calls:" << s_RenderStatsCache.DrawCalls << '\n';
				std::cout << "Max map bucket size: " << m_World->Map.MaxBucketSize() << '\n';
				std::cout << "===========================\n";
				break;
			case Quark::Key::O:
				m_Player.GetComponent<Quark::Transform3DComponent>().Position = { 0, 200, 0 };
				break;
			case Quark::Key::I:
				m_World->Loader.Invalidate();
				break;
			}

			return false;
		}

		bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e)
		{
			switch (e.GetMouseButton())
			{
			case Quark::MouseCode::ButtonLeft:
				if (!GetWindow().IsSelected())
				{
					GetWindow().Select();
					return true;
				}
				break;
			}

			PerformPlayerClick(e);

			return false;
		}

		bool OnWindowResized(Quark::WindowResizedEvent& e)
		{
			return false;
		}

		void OnWorldChunkLoaded(ChunkIdentifier id)
		{
			// Player feels gravity when chunk is loaded
			if (m_Player.GetPosition().ToChunkCoord() == id.Coord)
			{
				s_FlagPlayerGravity = true;
			}
		}

		void ProcessPlayerCollision()
		{
			static constexpr float detectionTreshold = 0.01f;
			const Position3D& playerPos = m_Player.GetPosition();
			const IntPosition3D blockUnderFeetPos = glm::floor(playerPos - Position3D(0.0f, detectionTreshold, 0.0f));

			const auto& props = m_World->GetBlock(blockUnderFeetPos).GetProperties();

			// Collide with block underneath
			if (props.CollisionEnabled)
			{
				const auto& blockBounds = props.Hitbox.MoveTo(blockUnderFeetPos).GetBounds();

				auto data = m_Player.GetHitbox().CollideWith(blockBounds);
				//auto data = m_Player.GetHitbox().CollideWith(glm::vec3(playerPos.x, blockBounds.Y.Max, playerPos.z));

				if (data.has_value())
				{
					auto& collision = data.value();

					m_Player.GetComponent<Quark::Transform3DComponent>().Position.y = blockBounds.Y.Max;
					m_Player.GetComponent<Quark::PhysicsComponent>().Velocity.y = 0.f;
				}
			}
		}

		void UpdateLoader()
		{
			const Position3D& playerPos = m_Player.GetPosition();
			auto& coord = m_World->Loader.Coord;
			coord = playerPos.ToChunkCoord();

			static ChunkCoord lastCoord = coord;
			if (lastCoord != coord)
			{
				m_World->Loader.Invalidate();
			}
			lastCoord = coord;
		}

		bool PerformPlayerClick(Quark::MouseButtonPressedEvent& e)
		{
			Ray ray;
			ray.Origin = m_Player.GetHeadPosition();
			ray.Direction = glm::normalize(m_Player.GetComponent<Quark::Transform3DComponent>().GetFrontVector());

			auto data = m_World->RayCast(ray.Origin, ray.Direction, 5.0f);
			if (data.has_value())
			{
				auto& collision = data.value();

				switch (e.GetMouseButton())
				{
				case Quark::MouseCode::ButtonLeft:
					if (collision.Block != BlockID::Air)
					{
						m_World->ReplaceBlock(collision.Impact, BlockID::Air);
					}
					break;
				case Quark::MouseCode::ButtonRight:
					if (collision.Block != BlockID::Air)
					{
						auto pos = collision.Impact + (Position3D)GetFaceNormal(collision.Side);
						if (m_World->GetBlock(pos) == BlockID::Air)
						{
							m_World->ReplaceBlock(pos, BlockID::Cobblestone);
						}
					}
					break;
				}
			}

			return false;
		}

		void CalculateAppPerformance(float elapsedTime)
		{
			static float accumFrameTime = 0.0f;
			static uint32_t frameCount = 0;

			accumFrameTime += elapsedTime;
			frameCount++;

			if (accumFrameTime >= 1.0f)
			{
				std::cout << std::fixed << "Avg FPS: " << ((float)frameCount / accumFrameTime) << " | Avg Time: " << (accumFrameTime / (float)frameCount) * 1000.0f << "ms\n";

				accumFrameTime = 0.0f;
				frameCount = 0;
			}
		}

	private:
		PlayerSettings m_PlayerSettings;

		Quark::Scope<World> m_World = World::Create(m_PlayerSettings.RenderDistance);

		Player m_Player = { m_World.get(), m_World->Scene };
		PlayerController m_Controller = { m_Player };
	};
}

int main()
{
	auto app = new VoxelCraft::VoxelCraft();
	app->Run();
	delete app;

	return 0;
}
