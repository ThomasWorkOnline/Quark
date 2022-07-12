#include "VulkanApp.h"

#include "Quark/Scripts/CameraController.h"

VulkanApp::VulkanApp()
{
	m_Scene = Scene::Create();
	m_CameraEntity = m_Scene->CreateEntity();

	m_CameraEntity.AddComponent<Transform3DComponent>().Position = { 0.0f, 0.0f, -1.0f };
	m_CameraEntity.AddComponent<PhysicsComponent>().Friction = 4.0f;
	m_CameraEntity.AddComponent<CameraComponent>().Camera.SetPerspective(90.0f);
	m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

	m_Scene->SetPrimaryCamera(m_CameraEntity);
	SceneRenderer::SetContext(m_Scene);
}

void VulkanApp::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(OnMouseButtonPressed));
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(OnKeyPressed));
	
	if (!e.Handled && m_ViewportSelected)
		m_Scene->OnEvent(e);
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
