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

	Random<bool> randomBool;
	Random<float> randomFloat;
	auto random = [&]() -> auto { return randomFloat.Next() * 100.0f; };

	for (uint32_t i = 0; i < 10000; i++)
	{
		auto sprite = m_Scene->CreateEntity();
		sprite.AddComponent<Transform3DComponent>().Position = { random(), random(), random() };
		auto& src = sprite.AddComponent<SpriteRendererComponent>();
		src.Texture = randomBool.Next() ? texture1 : texture2;
		src.Color = { 1.0f, 0.0f, 0.0, 1.0f };
	}

	QK_CORE_INFO("Texture 1 use count: {0}", texture1.use_count());
	QK_CORE_INFO("Texture 2 use count: {0}", texture2.use_count());

	m_SceneRenderer.SetContext(m_Scene);
}

void VulkanApp::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(OnMouseButtonPressed));
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<WindowResizedEvent>([&](WindowResizedEvent& e)
	{
		m_SceneRenderer.OnViewportResized(e.GetWidth(), e.GetHeight());
		return false;
	});

	if (!e.Handled && m_ViewportSelected)
		m_Scene->OnEvent(e);
}

void VulkanApp::OnRender()
{
	m_SceneRenderer.OnRender();
}

void VulkanApp::OnUpdate(Timestep elapsedTime)
{
	m_Scene->OnUpdate(elapsedTime);
}

bool VulkanApp::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	bool wasSelected = GetWindow()->IsCursorEnabled();
	if (e.GetMouseButton() == MouseCode::ButtonLeft)
	{
		m_ViewportSelected = true;
		GetWindow()->DisableCursor();
	}

	return wasSelected;
}

bool VulkanApp::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::Escape:
		{
			bool wasDeselected = !GetWindow()->IsCursorEnabled();
			m_ViewportSelected = false;
			GetWindow()->EnableCursor();
			return wasDeselected;
		}
	}

	return false;
}
