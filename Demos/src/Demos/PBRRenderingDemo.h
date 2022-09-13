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
	struct Material
	{
		Scope<Texture2D> Albedo;
		Scope<Texture2D> Metallic;
		Scope<Texture2D> Normal;
		Scope<Texture2D> Roughness;
		Scope<Texture2D> AO;
	};

	struct CameraUniformBufferData
	{
		Mat4f ViewProjection;
		Vec4f CameraPosition;
	};

	Scope<PresentableScene> m_Scene;
	Entity m_CameraEntity;
	Mesh m_Body;
	Material m_Material;

	Scope<Shader> m_PBRShader;
	Scope<Cubemap> m_Irradiance;
	Scope<UniformBuffer> m_CameraUniformBuffer;
	CameraUniformBufferData m_CameraBufferData;
	Scope<Pipeline> m_Pipeline;

	const UniformBuffer* m_UniformBuffer;

	std::future<OBJMeshData> m_MeshDataFuture;
	std::future<Scope<Image>> m_AlbedoFuture;
	std::future<Scope<Image>> m_MetallicFuture;
	std::future<Scope<Image>> m_NormalFuture;
	std::future<Scope<Image>> m_RoughnessFuture;
	std::future<Scope<Image>> m_AOFuture;
};
