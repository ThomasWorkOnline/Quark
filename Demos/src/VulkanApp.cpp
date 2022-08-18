#include "VulkanApp.h"
#include "Quark/Scripts/CameraController.h"

VulkanApp::VulkanApp(const ApplicationOptions& options)
	: Application(options)
{
	m_Scene = CreateRef<Scene>();
	m_CameraEntity = m_Scene->CreatePrimaryCamera();
	m_CameraEntity.AddNativeScript<CameraController>();

	Ref<Texture2D> texture1 = Texture2D::Create("assets/textures/pbr/streaked-metal/albedo.png");
	Ref<Texture2D> texture2 = Texture2D::Create("assets/textures/pbr/copper-rock/copper-rock1-alb.png");

	m_Font = Font::Create("assets/fonts/arial.ttf", 64);
	m_Text.SetFont(m_Font);

	Random<bool> randomBool;
	Random<float> randomFloat;
	auto random = [&]() -> auto { return randomFloat.Next() * 1000.0f; };

	for (uint32_t i = 0; i < 50000; i++)
	{
		auto sprite = m_Scene->CreateEntity();

		auto& transform = sprite.AddComponent<Transform3DComponent>();

		Vec3f axis = { randomFloat.Next(), randomFloat.Next(), randomFloat.Next() };
		transform.Position = Vec3f{ random(), random(), random() };
		transform.Scale = axis;
		transform.Rotate(randomFloat.Next() * glm::radians(360.0f), axis);

		auto& src = sprite.AddComponent<TexturedSpriteRendererComponent>();
		src.Texture = randomBool.Next() ? texture1 : texture2;
		src.Tint = { axis, 1.0f };
	}

	m_SceneRenderer.SetContext(m_Scene);

	auto window = GetWindow();
	m_TextCamera.SetOrthographic((float)window->GetWidth());
	m_TextCamera.Resize(window->GetWidth(), window->GetHeight());
}

void VulkanApp::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(OnMouseButtonPressed));
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<WindowResizedEvent>([&](WindowResizedEvent& e)
	{
		m_SceneRenderer.OnViewportResized(e.GetWidth(), e.GetHeight());
	});

	if (!e.Handled && m_ViewportSelected)
		m_Scene->OnEvent(e);
}

void VulkanApp::OnRender()
{
	m_SceneRenderer.OnRender();

	Renderer2D::BeginScene(m_TextCamera.GetProjection(), Mat4f(1.f));

	auto& pos = m_CameraEntity.GetComponent<Transform3DComponent>().Position;
	auto posStr = glm::to_string(pos);
	Renderer2D::DrawText(posStr, m_Font.get());

	Renderer2D::EndScene();
}

void VulkanApp::OnUpdate(Timestep elapsedTime)
{
	m_Scene->OnUpdate(elapsedTime);
}

void VulkanApp::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	if (e.GetMouseButton() == MouseCode::ButtonLeft)
	{
		m_ViewportSelected = true;
		GetWindow()->DisableCursor();
	}
}

void VulkanApp::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::Escape:
		{
			m_ViewportSelected = false;
			GetWindow()->EnableCursor();
		}
	}
}
