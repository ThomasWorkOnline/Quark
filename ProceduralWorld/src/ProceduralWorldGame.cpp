#include <Quark.h>

#include "PlayerCameraController.h"

class ProceduralWorldGame : public Quark::Application
{
public:
	void OnCreate() override
	{
		GetWindow().SetVSync(false);
		GetWindow().SetFullScreen(true);

		{
			m_Maxwell = m_Scene.CreateEntity();
			auto& transform = m_Maxwell.AddComponent<Quark::Transform3DComponent>();
			transform.Scale.x = (float)m_Texture->GetWidth() / (float)m_Texture->GetHeight();

			auto& physics = m_Maxwell.AddComponent<Quark::PhysicsComponent>();
			physics.Friction = 8.0f;
		}

		{
			m_Camera = m_Scene.CreateEntity();
			m_Camera.AddComponent<Quark::OrthographicCameraComponent>((float)GetWindow().GetWidth() / (float)GetWindow().GetHeight(), 4.0f);
			m_Camera.AddComponent<Quark::Transform3DComponent>();
			m_Camera.AddComponent<Quark::PhysicsComponent>();
		}

		m_CameraController.AttachCamera(m_Camera);
	}

	void OnUpdate(float elapsedTime) override
	{
		constexpr uint32_t movementSpeed = 80.0f;
		auto& playerPhysics = m_Maxwell.GetComponent<Quark::PhysicsComponent>();
		auto& playerTransform = m_Maxwell.GetComponent<Quark::Transform3DComponent>();
		
		if (Quark::Input::IsKeyPressed(Quark::Key::W))
			playerPhysics.Velocity.y += movementSpeed * elapsedTime;

		if (Quark::Input::IsKeyPressed(Quark::Key::A))
			playerPhysics.Velocity.x -= movementSpeed * elapsedTime;

		if (Quark::Input::IsKeyPressed(Quark::Key::S))
			playerPhysics.Velocity.y -= movementSpeed * elapsedTime;

		if (Quark::Input::IsKeyPressed(Quark::Key::D))
			playerPhysics.Velocity.x += movementSpeed * elapsedTime;

		m_CameraController.OnUpdate(elapsedTime, m_Maxwell);

		m_Scene.OnUpdate(elapsedTime);
		
		Quark::Renderer::BeginScene(m_Camera.GetComponent<Quark::OrthographicCameraComponent>().Camera.GetMatrix(),
			m_Camera.GetComponent<Quark::Transform3DComponent>());

		Quark::Renderer::SubmitSprite(m_Texture, playerTransform);

		constexpr int32_t scale = 50;
		static Quark::Transform3DComponent trans;
		for (int y = 0; y < scale; y++)
		{
			for (int x = 0; x < scale; x++)
			{
				trans.Position = { y - scale * 0.5f, x - scale * 0.5f, 0.0f };
				Quark::Renderer::SubmitSprite({ x / (float)scale, y / (float)scale, 0.0f, 1.0f }, trans);
			}
		}

		Quark::Renderer::EndScene();
	}

	void OnEvent(Quark::Event& e)
	{
		Quark::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(ProceduralWorldGame::OnKeyPressed));

		m_CameraController.OnEvent(e);
	}

	bool OnKeyPressed(Quark::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Quark::Key::F11:
		{
			GetWindow().SetFullScreen(!GetWindow().IsFullscreen());
			break;
		}
		}
		return false;
	}

private:
	Quark::Scene m_Scene;
	Quark::Entity m_Maxwell;
	Quark::Entity m_Camera;

	Quark::Ref<Quark::Texture2D> m_Texture = Quark::Texture2D::Create("assets/textures/Maxwell.png");
	Quark::Ref<Quark::Texture2D> m_Floor = Quark::Texture2D::Create("assets/textures/cobblestone.png");

	PlayerCameraController m_CameraController;

	Quark::Text m_Text = { "" };
};

int main()
{
	auto app = new ProceduralWorldGame();
	app->Run();
	delete app;

	return 0;
}
