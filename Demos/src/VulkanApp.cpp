#include "VulkanApp.h"

#include "Quark/Scripts/CameraController.h"

VulkanApp::VulkanApp()
{
	m_Scene = Scene::Create();
	m_CameraEntity = m_Scene->CreatePrimaryCamera();
	m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

	m_SceneRenderer.SetContext(m_Scene);
}

void VulkanApp::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(OnMouseButtonPressed));
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(OnWindowResized));
	
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
	bool wasSelected = GetWindow().IsCursorEnabled();
	if (e.GetMouseButton() == MouseCode::ButtonLeft)
	{
		m_ViewportSelected = true;
		GetWindow().DisableCursor();
	}

	return wasSelected;
}

bool VulkanApp::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::Escape:
		{
			bool wasDeselected = !GetWindow().IsCursorEnabled();
			m_ViewportSelected = false;
			GetWindow().EnableCursor();
			return wasDeselected;
		}
	}

	return false;
}

bool VulkanApp::OnWindowResized(WindowResizedEvent& e)
{
	m_SceneRenderer.OnViewportResized(e.GetWidth(), e.GetHeight());
	return false;
}
