#pragma once

#include <Quark.h>
#include <future>

using namespace Quark;

class PBRRenderingDemo : public Application
{
public:
	PBRRenderingDemo();

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
	Ref<Scene> m_Scene;
	Entity m_Player;

	Ref<Texture2D> m_Albedo;
	Ref<Texture2D> m_Metallic;
	Ref<Texture2D> m_Normal;
	Ref<Texture2D> m_Roughness;
	Ref<Texture2D> m_AO;

	Scope<Shader> m_PBRShader;
	Mesh m_Body;

	std::future<OBJMeshData> m_MeshDataFuture;
	std::future<Ref<Image>> m_AlbedoFuture;
	std::future<Ref<Image>> m_MetallicFuture;
	std::future<Ref<Image>> m_NormalFuture;
	std::future<Ref<Image>> m_RoughnessFuture;
	std::future<Ref<Image>> m_AOFuture;
};
