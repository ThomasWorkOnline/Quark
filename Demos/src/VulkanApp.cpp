#include "VulkanApp.h"

#include "Quark/Scripts/CameraController.h"

VulkanApp::VulkanApp(const ApplicationOptions& options)
	: Application(options)
{
	m_Scene = CreateRef<Scene>();
	m_CameraEntity = m_Scene->CreatePrimaryCamera();
	m_CameraEntity.AddNativeScript<CameraController>();

	auto sprite = m_Scene->CreateEntity();
	sprite.AddComponent<SpriteRendererComponent>().Color = { 1.0f, 0.0f, 0.0, 1.0f };
	sprite.AddComponent<Transform3DComponent>().Position = { 0.0f, 0.0f, 1.0f };

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
