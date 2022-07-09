#pragma once

#include <Quark.h>

using namespace Quark;

class VulkanApp : public Application
{
public:
	VulkanApp();
	virtual ~VulkanApp() override;

	virtual void OnEvent(Event& e) override;
	virtual void OnUpdate(Timestep elapsedTime) override;
	virtual void OnRender() override;

private:
	bool OnWindowResized(WindowResizedEvent& e);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	bool OnKeyPressed(KeyPressedEvent& e);

private:
	Ref<Scene> m_Scene;
	Entity m_CameraEntity;

	bool m_ViewportSelected = false;
	SceneRenderer m_SceneRenderer;
};
