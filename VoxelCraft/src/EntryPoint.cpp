#include <Entropy.h>

#include "World.h"

class VoxelCraft : public Entropy::Application
{
public:
	void OnCreate() override
	{
		m_Shader = m_ShaderDefault;

		{
			m_Player = m_Scene.CreateEntity();
			m_Player.AddComponent<Entropy::PerspectiveCameraComponent>((float)GetWindow().GetWidth() / (float)GetWindow().GetHeight(), 70.0f);
			auto& transform = m_Player.AddComponent<Entropy::Transform3DComponent>();
			transform.Position.z = -2.0f;
			transform.Position.y = 65.0f;
			m_Player.AddComponent<Entropy::PhysicsComponent>();
		}

		GetWindow().Select();
		GetWindow().SetVSync(false);
		GetWindow().SetFullScreen(false);
	}

	void OnUpdate(float elapsedTime) override
	{
		m_Scene.OnUpdate(elapsedTime);
		m_Controller.OnUpdate(elapsedTime);

		Entropy::Renderer::BeginScene(m_Player.GetComponent<Entropy::PerspectiveCameraComponent>().Camera.GetMatrix(),
			m_Player.GetComponent<Entropy::Transform3DComponent>());

		for (auto& chunk : m_World.GetChunks())
		{
			chunk.PushData();
			Entropy::Renderer::Submit(m_Shader, m_Stone, chunk.GetVertexArray());
		}

		Entropy::Renderer::EndScene();
	}

	void OnEvent(Entropy::Event& e) override
	{
		Entropy::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Entropy::KeyPressedEvent>(NT_ATTACH_EVENT_FN(VoxelCraft::OnKeyPressed));
		dispatcher.Dispatch<Entropy::MouseButtonPressedEvent>(NT_ATTACH_EVENT_FN(VoxelCraft::OnMouseButtonPressed));
		dispatcher.Dispatch<Entropy::WindowCloseEvent>(NT_ATTACH_EVENT_FN(VoxelCraft::OnWindowClose));

		m_Controller.OnEvent(e);
	}

	bool OnKeyPressed(Entropy::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Entropy::Key::Escape:
			GetWindow().Deselect();
			break;
		case Entropy::Key::X:
			if (*m_Shader == *m_ShaderDebug)
				m_Shader = m_ShaderDefault;
			else
				m_Shader = m_ShaderDebug;
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
		switch (e.GetMouseButton())
		{
		case Entropy::MouseCode::ButtonLeft:
			GetWindow().Select();
			break;
		default:
			break;
		}

		return false;
	}

	bool OnWindowClose(Entropy::WindowCloseEvent& e)
	{
		std::cout << "Shutdown world\n";
		m_World.Shutdown();
		return false;
	}

private:
	Entropy::Scene m_Scene;
	Entropy::Entity m_Player;
	Entropy::PerspectiveCameraController m_Controller = { m_Player };

	Entropy::Ref<Entropy::Shader> m_ShaderDebug = Entropy::Shader::Create("assets/shaders/debugNormal.glsl");
	Entropy::Ref<Entropy::Shader> m_ShaderDefault = Entropy::Shader::Create("assets/shaders/default.glsl");
	Entropy::Ref<Entropy::Shader> m_Shader;

	Entropy::Ref<Entropy::Texture2D> m_Stone = Entropy::Texture2D::Create("assets/textures/stone.png");

	World m_World;
};

int main()
{
	auto app = new VoxelCraft();
	app->Run();
	delete app;

	return 0;
}
