#pragma once

#include <Quark.h>

using namespace Quark;

class VulkanApp : public Application
{
public:
	VulkanApp(const ApplicationOptions& options = {});

	virtual void OnEvent(Event& e) override;
	virtual void OnRender() override;
	virtual void OnUpdate(Timestep elapsedTime) override;

private:
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	bool OnKeyPressed(KeyPressedEvent& e);

private:
	Ref<Scene> m_Scene;
	Entity m_CameraEntity;
	Ref<Font> m_Font;

	Text m_Text;
	SceneCamera m_TextCamera;

	SceneRenderer m_SceneRenderer;
	bool m_ViewportSelected = false;
};
