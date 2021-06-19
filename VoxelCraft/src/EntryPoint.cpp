#include <Quark.h>

#include "Chunk.h"
#include "ChunkRenderer.h"
#include "Player.h"
#include "PlayerController.h"
#include "World.h"
#include "Resources.h"

class VoxelCraft : public Quark::Application
{
public:
	void OnCreate() override
	{
		QK_TIME_SCOPE_DEBUG(VoxelCraft::OnCreate);

		GetWindow().Select();
		GetWindow().SetVSync(true);
		GetWindow().SetFullScreen(false);
	}

	void OnDestroy() override
	{
		
	}

	void OnUpdate(float elapsedTime) override
	{
		Quark::RenderCommand::SetClearColor(Quark::EncodeSRGB(glm::vec4(0.78f, 0.90f, 0.93f, 1.0f)));
		Quark::RenderCommand::Clear();

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
		Ray ray;
		ray.Origin = m_World.GetPlayer().GetPosition();
		ray.Direction = glm::normalize(m_World.GetPlayer().GetTransform().GetFrontVector());

		auto collisionData = m_World.RayCast(ray.Origin, ray.Direction, 5.0f);

		switch (e.GetMouseButton())
		{
		case Quark::MouseCode::ButtonLeft:
			GetWindow().Select();

			if (collisionData.Block != Block::None && collisionData.Block != Block::Air)
			{
				m_World.ReplaceBlock(collisionData.Impact, Block::Air);
			}
			break;
		case Quark::MouseCode::ButtonRight:
		{
			if (collisionData.Block != Block::None && collisionData.Block != Block::Air)
			{
				m_World.ReplaceBlock(collisionData.Impact + (glm::vec3)GetFaceNormal(collisionData.Side), Block::Cobblestone);
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
