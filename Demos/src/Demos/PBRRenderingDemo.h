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
	struct CameraUniformBufferData
	{
		Mat4f ViewProjection;
	};

	Ref<Scene> m_Scene;
	Entity m_Player;

	Scope<Texture2D> m_Albedo;
	Scope<Texture2D> m_Metallic;
	Scope<Texture2D> m_Normal;
	Scope<Texture2D> m_Roughness;
	Scope<Texture2D> m_AO;

	Scope<Shader> m_PBRShader;
	Scope<UniformBuffer> m_UniformBuffer;
	CameraUniformBufferData m_UniformBufferData;

	Mesh m_Body;

	std::future<OBJMeshData> m_MeshDataFuture;
	std::future<Scope<Image>> m_AlbedoFuture;
	std::future<Scope<Image>> m_MetallicFuture;
	std::future<Scope<Image>> m_NormalFuture;
	std::future<Scope<Image>> m_RoughnessFuture;
	std::future<Scope<Image>> m_AOFuture;
};
