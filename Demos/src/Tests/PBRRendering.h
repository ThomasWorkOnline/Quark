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

	Ref<Texture2D> m_Albedo;
	Ref<Texture2D> m_Metallic;
	Ref<Texture2D> m_Normal;
	Ref<Texture2D> m_Roughness;
	Ref<Texture2D> m_AmbiantOcclusion;

	Ref<Texture2D> m_HDRMap;
	Ref<Cubemap> m_HDRCubeMap;
	Ref<Framebuffer> m_IrradianceFramebuffer;

	Ref<Shader> m_PBRShader;
	Ref<Shader> m_IrradianceShader;
	Ref<Shader> m_EquirectangleToCubemap;
	Mesh m_Body;
};
