#include <Quark.h>

#include "Chunk.h"
#include "ChunkRenderer.h"
#include "Player.h"
#include "PlayerController.h"
#include "World.h"

class VoxelCraft : public Quark::Application
{
public:
	void OnCreate() override
	{
		QK_TIME_SCOPE_DEBUG(VoxelCraft::OnCreate);

		GetWindow().Select();
		GetWindow().SetVSync(false);
		GetWindow().SetFullScreen(false);
	}

	void OnUpdate(float elapsedTime) override
	{
		m_World.OnUpdate(elapsedTime);
		m_Controller.OnUpdate(elapsedTime);
	}

	void OnEvent(Quark::Event& e) override
	{
		Quark::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnKeyPressed));
		dispatcher.Dispatch<Quark::MouseButtonPressedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnMouseButtonPressed));

		m_World.OnEvent(e);
		m_Controller.OnEvent(e);
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
		default:
			break;
		}

		return false;
	}

	bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e)
	{
		auto [block, position, next] = m_World.RayCast(m_World.GetPlayer().GetHeadPosition(), m_World.GetPlayer().GetTransform().GetFrontVector(), 5.0f);

		switch (e.GetMouseButton())
		{
		case Quark::MouseCode::ButtonLeft:
			GetWindow().Select();

			if (block != BlockId::None && block != BlockId::Air)
			{
				m_World.ReplaceBlock(position, BlockId::Air);
			}
			break;
		case Quark::MouseCode::ButtonRight:
		{
			if (block != BlockId::None && block != BlockId::Air)
			{
				m_World.ReplaceBlock(position + next, BlockId::Cobblestone);
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
