#include <Entropy.h>

#include "World.h"
#include "Player.h"

class VoxelCraft : public Entropy::Application
{
public:
	void OnCreate() override
	{
		m_Shader = m_ShaderDefault;

		GetWindow().Select();
		GetWindow().SetVSync(false);
		GetWindow().SetFullScreen(false);
	}

	void OnUpdate(float elapsedTime) override
	{
		m_World.OnUpdate(elapsedTime);
		m_Controller.OnUpdate(elapsedTime);

		Entropy::Renderer::BeginScene(m_Player.GetCamera().Camera.GetMatrix(),
			m_Player.GetTransform());

		m_World.OnRender(m_Shader);
		
		Entropy::Renderer::EndScene();
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

private:
	World m_World;

	Player m_Player = { &m_World.GetScene() };
	Entropy::PerspectiveCameraController m_Controller = { m_Player };

	Entropy::Ref<Entropy::Shader> m_ShaderDebug = Entropy::Shader::Create("assets/shaders/debugNormal.glsl");
	Entropy::Ref<Entropy::Shader> m_ShaderDefault = Entropy::Shader::Create("assets/shaders/default.glsl");
	Entropy::Ref<Entropy::Shader> m_Shader;
};

int main()
{
	auto app = new VoxelCraft();
	app->Run();
	delete app;

	return 0;
}
