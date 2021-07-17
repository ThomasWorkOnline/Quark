#include <Quark.h>

#include "../World/ChunkIdentifier.h"
#include "../World/World.h"
#include "../Rendering/Renderer.h"
#include "Resources.h"

namespace VoxelCraft {

	class VoxelCraft : public Quark::Application
	{
	public:
		void OnCreate() override
		{
			QK_TIME_SCOPE_DEBUG(VoxelCraft::OnCreate);

			GetWindow().Select();
			GetWindow().SetVSync(false);
			GetWindow().SetFullScreen(false);

			Resources::Initialize();
			Renderer::Initialize();

			m_World = World::Create();
		}

		void OnDestroy() override
		{
			Renderer::Shutdown();
		}

		void OnUpdate(float elapsedTime) override
		{
			Quark::RenderCommand::SetClearColor(Quark::EncodeSRGB(glm::vec4(0.78f, 0.90f, 0.93f, 1.0f)));
			Quark::RenderCommand::Clear();

			m_World->OnUpdate(elapsedTime);

			//CalculateAppPerformance(elapsedTime);
		}

		void OnEvent(Quark::Event& e) override
		{
			Quark::EventDispatcher dispatcher(e);
			dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnKeyPressed));
			dispatcher.Dispatch<Quark::MouseButtonPressedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnMouseButtonPressed));
			dispatcher.Dispatch<Quark::WindowResizedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnWindowResized));

			if (!e.Handled)
			{
				m_World->OnEvent(e);
			}
		}

		bool OnKeyPressed(Quark::KeyPressedEvent& e)
		{
			switch (e.GetKeyCode())
			{
			case Quark::Key::Escape:
				GetWindow().Deselect();
				break;
			case Quark::Key::F11:
				GetWindow().SetFullScreen(!GetWindow().IsFullscreen());
				break;
			case Quark::KeyCode::F1:
				Renderer::SwitchShader();
				break;
			}

			return false;
		}

		bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e)
		{
			switch (e.GetMouseButton())
			{
			case Quark::MouseCode::ButtonLeft:
				bool selected = GetWindow().IsSelected();
				GetWindow().Select();
				return !selected;
			}

			return false;
		}

		bool OnWindowResized(Quark::WindowResizedEvent& e)
		{
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
		Quark::Scope<World> m_World;
	};
}

int main()
{
	auto app = new VoxelCraft::VoxelCraft();
	app->Run();
	delete app;

	return 0;
}
