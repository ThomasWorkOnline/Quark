#include "VulkanApp.h"
#include "Quark/Scripts/CameraController.h"

VulkanApp::VulkanApp(const ApplicationOptions& options)
	: Application(options)
{
	m_Scene = CreateRef<PresentableScene>();
	m_Scene->GetSettings().GlobalFrictionCoeff = 0.0f;
	m_CameraEntity = m_Scene->CreatePrimaryCamera();
	m_CameraEntity.AddNativeScript<CameraController>();

	Ref<Texture2D> texture1 = Texture2D::Create("assets/Textures/pbr/streaked-metal/albedo.png");
	Ref<Texture2D> texture2 = Texture2D::Create("assets/Textures/pbr/copper-rock/copper-rock1-alb.png");

	Random<bool> randomBool;
	Random<float> randomFloat;
	auto random = [&]() -> auto { return (randomFloat.Next() - 0.5f) * 1000.0f; };

	for (uint32_t i = 0; i < 10000; i++)
	{
		auto sprite = m_Scene->CreateEntity();

		Vec3f axis = { randomFloat.Next(), randomFloat.Next(), randomFloat.Next() };

		auto& transform = sprite.AddComponent<Transform3DComponent>();
		transform.Position = Vec3f{ random(), random(), random() };
		transform.Scale = axis * 10.f;
		transform.Rotate(randomFloat.Next() * glm::radians(360.0f), axis);

		auto& physics = sprite.AddComponent<PhysicsComponent>();
		physics.Velocity = axis;

		auto& src = sprite.AddComponent<TexturedSpriteRendererComponent>();
		src.Texture = randomBool.Next() ? texture1 : texture2;
		src.Tint = { randomFloat.Next(), randomFloat.Next(), randomFloat.Next(), 1.0f };
	}

	m_Font = Font::Create("assets/Fonts/arial.ttf", 48);
	m_Text.SetFont(m_Font);
	
	auto* window = GetWindow();
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
		m_TextCamera.Resize(e.GetWidth(), e.GetHeight());
	});

	e.Handled = !m_ViewportSelected && e.IsInCategory(EventCategory::Input);

	if (!e.Handled)
		m_Scene->OnEvent(e);

	DefaultEventHandler(e);
}

void VulkanApp::OnRender()
{
	m_Scene->OnRender();

	if (m_PositionOverlay)
	{
		glm::identity<glm::mat4>();
		Renderer2D::BeginScene(m_TextCamera.GetProjection(), Mat4f(1.f));

		auto& pos = m_CameraEntity.GetComponent<Transform3DComponent>().Position;
		auto posStr = glm::to_string(pos);
		Renderer2D::DrawText(posStr, m_Font.get(), { 0.0f, 1.0f, 1.0f, 0.8f });

		Renderer2D::EndScene();
	}
}

void VulkanApp::OnUpdate(Timestep elapsedTime)
{
	m_Scene->OnUpdate(elapsedTime);
}

void VulkanApp::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	switch (e.GetMouseButton())
	{
		case MouseCode::ButtonLeft:
		{
			m_ViewportSelected = true;
			GetWindow()->DisableCursor();
			break;
		}
		case MouseCode::ButtonRight:
		{
			m_PositionOverlay = !m_PositionOverlay;
			break;
		}
	}
}

void VulkanApp::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::Escape:
		{
			m_ViewportSelected = false;
			auto* window = GetWindow();
			window->EnableCursor();
			break;
		}
	}
}
