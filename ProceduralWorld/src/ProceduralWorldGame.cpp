#include <Entropy.h>

#include "PlayerCameraController.h"

class ProceduralWorldGame : public Entropy::Application
{
public:
	void OnCreate() override
	{
		GetWindow().SetVSync(false);
		GetWindow().SetFullScreen(true);

		{
			m_Maxwell = m_Scene.CreateEntity();
			auto& transform = m_Maxwell.AddComponent<Entropy::Transform3DComponent>();
			transform.Scale.x = (float)m_Texture->GetWidth() / (float)m_Texture->GetHeight();

			auto& physics = m_Maxwell.AddComponent<Entropy::PhysicsComponent>();
			physics.Friction = 8.0f;
		}

		{
			m_Camera = m_Scene.CreateEntity();
			m_Camera.AddComponent<Entropy::OrthographicCameraComponent>((float)GetWindow().GetWidth() / (float)GetWindow().GetHeight(), 4.0f);
			m_Camera.AddComponent<Entropy::Transform3DComponent>();
			m_Camera.AddComponent<Entropy::PhysicsComponent>();
		}

		m_CameraController.AttachCamera(m_Camera);
	}

	void OnUpdate(float elapsedTime) override
	{
		constexpr uint32_t movementSpeed = 80.0f;
		auto& playerPhysics = m_Maxwell.GetComponent<Entropy::PhysicsComponent>();
		auto& playerTransform = m_Maxwell.GetComponent<Entropy::Transform3DComponent>();
		
		if (Entropy::Input::IsKeyPressed(Entropy::Key::W))
			playerPhysics.Velocity.y += movementSpeed * elapsedTime;

		if (Entropy::Input::IsKeyPressed(Entropy::Key::A))
			playerPhysics.Velocity.x -= movementSpeed * elapsedTime;

		if (Entropy::Input::IsKeyPressed(Entropy::Key::S))
			playerPhysics.Velocity.y -= movementSpeed * elapsedTime;

		if (Entropy::Input::IsKeyPressed(Entropy::Key::D))
			playerPhysics.Velocity.x += movementSpeed * elapsedTime;

		m_CameraController.OnUpdate(elapsedTime, m_Maxwell);

		m_Scene.OnUpdate(elapsedTime);
		
		Entropy::Renderer::BeginScene(m_Camera.GetComponent<Entropy::OrthographicCameraComponent>().Camera.GetMatrix(),
			m_Camera.GetComponent<Entropy::Transform3DComponent>());

		Entropy::Renderer::SubmitSprite(m_Texture, playerTransform);

		constexpr int32_t scale = 50;
		static Entropy::Transform3DComponent trans;
		for (int y = 0; y < scale; y++)
		{
			for (int x = 0; x < scale; x++)
			{
				trans.Position = { y - scale * 0.5f, x - scale * 0.5f, 0.0f };
				Entropy::Renderer::SubmitSprite({ x / (float)scale, y / (float)scale, 0.0f, 1.0f }, trans);
			}
		}

		Entropy::Renderer::EndScene();
	}

	void OnEvent(Entropy::Event& e)
	{
		Entropy::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Entropy::KeyPressedEvent>(NT_ATTACH_EVENT_FN(ProceduralWorldGame::OnKeyPressed));

		m_CameraController.OnEvent(e);
	}

	bool OnKeyPressed(Entropy::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Entropy::Key::F11:
		{
			GetWindow().SetFullScreen(!GetWindow().IsFullscreen());
			break;
		}
		}
		return false;
	}

private:
	Entropy::Scene m_Scene;
	Entropy::Entity m_Maxwell;
	Entropy::Entity m_Camera;

	Entropy::Ref<Entropy::Texture2D> m_Texture = Entropy::Texture2D::Create("assets/textures/Maxwell.png");
	Entropy::Ref<Entropy::Texture2D> m_Floor = Entropy::Texture2D::Create("assets/textures/cobblestone.png");

	PlayerCameraController m_CameraController;

	Entropy::Text m_Text = { "" };
};

int main()
{
	auto app = new ProceduralWorldGame();
	app->Run();
	delete app;

	return 0;
}
