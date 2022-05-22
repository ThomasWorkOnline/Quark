#pragma once

#include <Quark.h>
#include <future>
#include <queue>

using namespace Quark;

class PBRRendering : public Application
{
public:
	PBRRendering();

	void OnUpdate(Timestep elapsedTime) override;
	void OnRender() override;
	void OnEvent(Event& e) override;

private:
	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnMouseMoved(MouseMovedEvent& e);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	void UploadAssets();

private:
	Scene m_Scene;
	Entity m_Player;
	PerspectiveCameraController m_Controller;

	Ref<Texture2D> m_Albedo;
	Ref<Texture2D> m_Metallic;
	Ref<Texture2D> m_Normal;
	Ref<Texture2D> m_Roughness;
	Ref<Texture2D> m_AO;

	Ref<Texture2D> m_HDRTexture;
	Ref<Cubemap> m_HDRCubemap;
	Ref<Cubemap> m_Irradiance;
	Ref<Framebuffer> m_EnvironmentFramebuffer;

	Ref<Shader> m_PBRShader;
	Ref<Shader> m_IrradianceShader;
	Ref<Shader> m_EquirectangleToCubemapShader;
	Ref<Shader> m_SkyboxShader;
	Mesh m_Body;
	Ref<VertexArray> m_CubemapVAO;

	std::future<OBJMeshData> m_MeshDataFuture;
	std::future<Ref<Image>> m_AlbedoFuture;
	std::future<Ref<Image>> m_MetallicFuture;
	std::future<Ref<Image>> m_NormalFuture;
	std::future<Ref<Image>> m_RoughnessFuture;
	std::future<Ref<Image>> m_AOFuture;
};
