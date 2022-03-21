#pragma once

#include <Quark.h>

using namespace Quark;

class PBRRendering : public Application
{
public:
	PBRRendering();

	void OnUpdate(Timestep elapsedTime) override;
	void OnEvent(Event& e) override;

private:
	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnMouseMoved(MouseMovedEvent& e);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

private:
	Scene m_Scene;
	Entity m_Player;
	PerspectiveCameraController m_Controller;

	TextureRenderModes m_TextureModes;
	Ref<Texture2D> m_Texture;

	Ref<Shader> m_Shader;
	Mesh m_Cube;
};
