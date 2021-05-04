#include <Entropy.h>

#include "Chunk.h"
#include "ChunkRenderer.h"
#include "Player.h"
#include "PlayerController.h"
#include "World.h"

class VoxelCraft : public Entropy::Application
{
public:
	void OnCreate() override
	{
		NT_TIME_SCOPE_DEBUG(VoxelCraft::OnCreate);

		GetWindow().Select();
		GetWindow().SetVSync(true);
		GetWindow().SetFullScreen(false);
	}

	void OnUpdate(float elapsedTime) override
	{
		m_World.OnUpdate(elapsedTime);
		m_Controller.OnUpdate(elapsedTime);

		{
			Entropy::Renderer::BeginScene(m_World.GetPlayer().GetCamera().Camera.GetMatrix(),
				m_World.GetPlayer().GetTransform());

			for (auto& chunk : m_World.GetRenderableChunks())
				ChunkRenderer::SubmitChunk(*chunk);

			Entropy::Renderer::EndScene();
		}
	}

	void OnEvent(Entropy::Event& e) override
	{
		Entropy::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Entropy::KeyPressedEvent>(NT_ATTACH_EVENT_FN(VoxelCraft::OnKeyPressed));
		dispatcher.Dispatch<Entropy::MouseButtonPressedEvent>(NT_ATTACH_EVENT_FN(VoxelCraft::OnMouseButtonPressed));

		m_Controller.OnEvent(e);
	}

	bool OnKeyPressed(Entropy::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Entropy::Key::Escape:
			GetWindow().Deselect();
			break;
		case Entropy::Key::F11:
			GetWindow().SetFullScreen(!GetWindow().IsFullscreen());
			break;
		default:
			break;
		}

		return false;
	}

	bool OnMouseButtonPressed(Entropy::MouseButtonPressedEvent& e)
	{
		auto [block, position, next] = m_World.RayCast(m_World.GetPlayer().GetHeadPosition(), m_World.GetPlayer().GetTransform().GetFrontVector(), 5.0f);

		switch (e.GetMouseButton())
		{
		case Entropy::MouseCode::ButtonLeft:
			GetWindow().Select();

			if (block != BlockId::None && block != BlockId::Air)
			{
				m_World.ReplaceBlockFromPositionAbsolute(position, BlockId::Air);
			}
			break;
		case Entropy::MouseCode::ButtonRight:
		{
			if (block != BlockId::None && block != BlockId::Air)
			{
				m_World.ReplaceBlockFromPositionAbsolute(position + next, BlockId::Cobblestone);
			}
			break;
		}
		}
		
		return false;
	}

private:
	World m_World;
	PlayerController m_Controller = { m_World.GetPlayer() };
};

int main()
{
	auto app = new VoxelCraft();
	app->Run();
	delete app;

	return 0;
}
