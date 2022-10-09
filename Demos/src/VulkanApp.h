#pragma once

#include <Quark.h>

using namespace Quark;

class VulkanApp : public Application
{
public:
	VulkanApp(const ApplicationOptions& options = {});
	~VulkanApp();

	virtual void OnEvent(Event& e) override;
	virtual void OnRender() override;
	virtual void OnUpdate(Timestep elapsedTime) override;

private:
	void OnMouseButtonPressed(MouseButtonPressedEvent& e);
	void OnKeyPressed(KeyPressedEvent& e);

private:
	Ref<PresentableScene> m_Scene;
	Entity m_CameraEntity;

	Ref<Font> m_Font;
	Text m_Text;
	SceneCamera m_TextCamera;

	bool m_ViewportSelected = false;
};
