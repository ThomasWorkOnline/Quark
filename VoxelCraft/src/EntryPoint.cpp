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
		GetWindow().Select();
		GetWindow().SetVSync(false);
		GetWindow().SetFullScreen(false);
	}

	void OnUpdate(float elapsedTime) override
	{
		m_World.OnUpdate(elapsedTime);
		m_Controller.OnUpdate(elapsedTime);

		{
			Entropy::Renderer::BeginScene(m_Player.GetCamera().Camera.GetMatrix(),
				m_Player.GetTransform());

			for (auto& chunk : m_World.GetChunks())
				ChunkRenderer::SubmitChunk(chunk);

			Entropy::Renderer::EndScene();
		}
		
		/*
		{
			glm::mat4 view = (glm::mat3)m_Player.GetTransform();
			Entropy::Renderer::BeginScene(m_Player.GetCamera().Camera.GetMatrix(), view);

			Entropy::RenderCommand::SetCullFace(Entropy::RenderCullFace::Back);
			Entropy::RenderCommand::SetDepthFunction(Entropy::RenderDepthFunction::LessEqual);

			m_Environment.GetCubeMap()->Attach(0);
			Entropy::Renderer::Submit(m_Shader, m_Environment.GetVertexArray());

			Entropy::RenderCommand::SetCullFace(Entropy::RenderCullFace::Front);
			Entropy::RenderCommand::SetDepthFunction(Entropy::RenderDepthFunction::Less);

			Entropy::Renderer::EndScene();
		}
		*/
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
		auto [block, position, next] = m_World.RayCast(m_Player.GetHeadPosition(), m_Player.GetTransform().GetFrontVector(), 5.0f);

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
		default:
			break;
		}
		
		return false;
	}

private:
	World m_World;

	Player m_Player = { &m_World.GetScene() };
	PlayerController m_Controller = { m_Player };

	Entropy::Environment m_Environment = { "assets/environments/Lycksele3" };
	Entropy::Ref<Entropy::Shader> m_Shader = Entropy::Shader::Create("assets/shaders/skybox.glsl");
};

int main()
{
	auto app = new VoxelCraft();
	app->Run();
	delete app;

	return 0;
}
