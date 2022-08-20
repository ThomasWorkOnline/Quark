#pragma once

#include <Quark.h>

#include <future>

using namespace Quark;

class PBRRenderingDemo : public Application
{
public:
	PBRRenderingDemo(const ApplicationOptions& options);

	void OnUpdate(Timestep elapsedTime) override;
	void OnRender() override;
	void OnEvent(Event& e) override;

private:
	bool OnMouseMoved(MouseMovedEvent& e);
	void OnMouseButtonPressed(MouseButtonPressedEvent& e);
	void OnKeyPressed(KeyPressedEvent& e);

	void LoadMaterialsAsync();
	void UploadAssets();

private:
	struct Light
	{
		Vec4f Color;
		Vec4f Position;
	};

	struct CameraUniformBufferData
	{
		Mat4f ViewProjection;
	};

	Ref<PresentableScene> m_Scene;
	Entity m_CameraEntity;
	Mesh m_Body;

	Scope<Texture2D> m_Albedo;
	Scope<Texture2D> m_Metallic;
	Scope<Texture2D> m_Normal;
	Scope<Texture2D> m_Roughness;
	Scope<Texture2D> m_AO;
	Scope<Cubemap> m_Irradiance;

	Scope<Shader> m_PBRShader;
	Scope<UniformBuffer> m_UniformBuffer;
	CameraUniformBufferData m_UniformBufferData;
	Scope<Pipeline> m_Pipeline;

	std::future<OBJMeshData> m_MeshDataFuture;
	std::future<Scope<Image>> m_AlbedoFuture;
	std::future<Scope<Image>> m_MetallicFuture;
	std::future<Scope<Image>> m_NormalFuture;
	std::future<Scope<Image>> m_RoughnessFuture;
	std::future<Scope<Image>> m_AOFuture;
};
