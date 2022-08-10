#include "qkpch.h"
#include "SceneRenderer.h"

#include "Quark/Core/Application.h"
#include "Quark/Filesystem/Filesystem.h"

#include "Renderer.h"
#include "RenderCommand.h"

#include <glad/glad.h>

namespace Quark {

	struct Vertex2D
	{
		Vec3f Position;
		Vec3f Color;
	};

	static constexpr Vertex2D s_Vertices[4] = {
		{ { -0.5f, -0.5f, 1.0f }, { 1.0f,  0.0f,  0.0f } },
		{ {  0.5f, -0.5f, 1.0f }, { 0.0f,  1.0f,  0.0f } },
		{ {  0.5f,  0.5f, 1.0f }, { 0.0f,  0.0f,  1.0f } },
		{ { -0.5f,  0.5f, 1.0f }, { 1.0f,  1.0f,  1.0f } }
	};

	static constexpr uint32_t s_Indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	static BufferLayout s_Vertex2DLayout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float3, "a_Color"    }
	};

	void SceneRenderer::SetContext(Ref<Scene> scene)
	{
		QK_CORE_ASSERT(scene, "Scene is nullptr!");

		m_Scene = std::move(scene);

		if (m_Scene->HasPrimaryCamera())
		{
			auto& viewport = Renderer::GetViewportExtent();

			auto& cc = m_Scene->m_PrimaryCameraEntity.GetComponent<CameraComponent>();
			cc.Camera.Resize(viewport.Width, viewport.Height);
		}
	}

	SceneRenderer::SceneRenderer()
	{
		Initialize();
	}

	SceneRenderer::~SceneRenderer()
	{
		m_Data.GraphicsPipeline.reset();
	}

	void SceneRenderer::OnRender()
	{
		if (m_Scene && m_Scene->HasPrimaryCamera())
		{
			auto& sceneCamera = m_Scene->m_PrimaryCameraEntity.GetComponent<CameraComponent>().Camera;
			auto& cameraTransform = m_Scene->m_PrimaryCameraEntity.GetComponent<Transform3DComponent>();

			Mat4f rotate = glm::toMat4(cameraTransform.Orientation);
			Mat4f view = glm::translate(rotate, (Vec3f)-cameraTransform.Position);

			m_Data.CameraBufferData.View = view;
			m_Data.CameraBufferData.Projection = sceneCamera.GetProjection();

			Renderer::BindPipeline(m_Data.GraphicsPipeline.get());
			m_Data.GraphicsPipeline->GetUniformBuffer()->SetData(&m_Data.CameraBufferData, sizeof(SceneData::CameraUniformBufferData));

			GeometryPass();

			if (m_Data.Env)
			{
				RenderCommand::SetCullFace(RenderCullMode::Front);
				RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

				m_Data.Env->SkyboxShader->Attach();
				m_Data.Env->SkyboxShader->SetMat4f("u_View", view);
				m_Data.Env->SkyboxShader->SetMat4f("u_Projection", sceneCamera.GetProjection());
				m_Data.Env->SkyboxShader->SetInt("u_EnvironmentMap", 0);
				m_Data.Env->SkyboxShader->SetFloat("u_Exposure", 1.0f);

				m_Data.Env->Environment->Attach(0);
				m_Data.Env->Irradiance->Attach(5); // TODO: put inside scene uniform buffer

				Renderer::Submit(m_Data.Env->CubemapVertexBuffer.get(), m_Data.Env->CubemapIndexBuffer.get());
				RenderCommand::SetCullFace(RenderCullMode::Default);
				RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
			}
		}
	}

	void SceneRenderer::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Data.GraphicsPipeline->Resize(viewportWidth, viewportHeight);

		if (m_Scene && m_Scene->HasPrimaryCamera())
		{
			auto& cc = m_Scene->m_PrimaryCameraEntity.GetComponent<CameraComponent>();
			cc.Camera.Resize(viewportWidth, viewportHeight);
		}
	}

	void SceneRenderer::SetEnvironment(std::string_view filepath)
	{
		NewEnvironment(filepath);
	}

	void SceneRenderer::GeometryPass()
	{
		Renderer::BeginGeometryPass();
		Renderer::Submit(m_Data.VertexBuffer.get(), m_Data.IndexBuffer.get());
		Renderer::EndRenderPass();
	}

	void SceneRenderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		if (GraphicsAPI::GetAPI() == RHI::Vulkan)
		{
			std::string vertexSource = Filesystem::ReadTextFile("../Quark/assets/shaders/version/4.50/bin/shader.vert.spv");
			std::string fragmentSource = Filesystem::ReadTextFile("../Quark/assets/shaders/version/4.50/bin/shader.frag.spv");

			m_Data.Shader.reset(Shader::Create("shader", vertexSource, fragmentSource));
		}
		else if (GraphicsAPI::GetAPI() == RHI::OpenGL)
		{
			std::string vertexSource = Filesystem::ReadTextFile("../Quark/assets/shaders/version/4.50/shader.vert");
			std::string fragmentSource = Filesystem::ReadTextFile("../Quark/assets/shaders/version/4.50/shader.frag");

			m_Data.Shader.reset(Shader::Create("shader", vertexSource, fragmentSource));
		}

		{
			auto& viewport = Renderer::GetViewportExtent();

			PipelineSpecification spec;
			spec.ViewportWidth = viewport.Width;
			spec.ViewportHeight = viewport.Height;
			spec.CameraUniformBufferSize = sizeof(SceneData::CameraUniformBufferData);
			spec.Layout = s_Vertex2DLayout;
			spec.RenderPass = Renderer::GetGeometryPass();
			spec.Shader = m_Data.Shader.get();

			m_Data.GraphicsPipeline.reset(Pipeline::Create(spec));
		}

		{
			m_Data.VertexBuffer.reset(VertexBuffer::Create(s_Vertices, sizeof(s_Vertices)));
			m_Data.VertexBuffer->SetLayout(s_Vertex2DLayout);

			m_Data.IndexBuffer.reset(IndexBuffer::Create(s_Indices, sizeof(s_Indices) / sizeof(uint32_t)));
		}
	}

	void SceneRenderer::NewEnvironment(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		if (!m_Data.Env)
		{
			auto& assetDir = Application::Get()->GetOptions().AssetDir;

			m_Data.Env = CreateScope<EnvironmentData>();
			m_Data.Env->SkyboxShader.reset(Shader::Create((assetDir / "assets/shaders/version/3.30/cubemap.glsl").string()));
			m_Data.Env->IrradianceShader.reset(Shader::Create((assetDir / "assets/shaders/version/3.30/irradiance.glsl").string()));
			m_Data.Env->EquirectangleToCubemapShader.reset(Shader::Create((assetDir / "assets/shaders/version/3.30/equirectangle_to_cubemap.glsl").string()));

			{
				Mesh cube = Mesh::GenerateUnitCube();
				m_Data.Env->CubemapVertexBuffer.reset(cube.GetVertexBuffer());
				m_Data.Env->CubemapIndexBuffer.reset(cube.GetIndexBuffer());
			}

			{
				CubemapSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				m_Data.Env->Environment.reset(Cubemap::Create(spec));

				spec.Width = 32;
				spec.Height = 32;
				m_Data.Env->Irradiance.reset(Cubemap::Create(spec));
			}

			{
				FramebufferAttachmentSpecification attachmentSpec = {
					ColorDataFormat::RGBA16f
				};

				FramebufferSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				spec.Attachments = { Ref<FramebufferAttachment>(FramebufferAttachment::Create(nullptr, attachmentSpec)) };
				m_Data.Env->Framebuffer.reset(Framebuffer::Create(spec));
			}
		}

		Mat4f captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		Mat4f captureViews[] = {
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 1.0f,  0.0f,  0.0f), Vec3f(0.0f, -1.0f,  0.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(-1.0f,  0.0f,  0.0f), Vec3f(0.0f, -1.0f,  0.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 0.0f,  1.0f,  0.0f), Vec3f(0.0f,  0.0f, -1.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 0.0f, -1.0f,  0.0f), Vec3f(0.0f,  0.0f,  1.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 0.0f,  0.0f, -1.0f), Vec3f(0.0f, -1.0f,  0.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 0.0f,  0.0f,  1.0f), Vec3f(0.0f, -1.0f,  0.0f))
		};

		auto hdrTexture = Texture2D::Create(filepath);
		hdrTexture->Attach(0);

		m_Data.Env->Framebuffer->Attach();
		m_Data.Env->EquirectangleToCubemapShader->Attach();
		m_Data.Env->EquirectangleToCubemapShader->SetInt("u_EquirectangularMap", 0);
		m_Data.Env->EquirectangleToCubemapShader->SetMat4f("u_Projection", captureProjection);

		RenderCommand::SetCullFace(RenderCullMode::Front);
		RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

		for (uint8_t i = 0; i < 6; i++)
		{
			m_Data.Env->EquirectangleToCubemapShader->SetMat4f("u_View", captureViews[i]);
			//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Environment->GetRendererID());

			// FIX:
			RenderCommand::Clear();
			Renderer::Submit(m_Data.Env->CubemapVertexBuffer.get(), m_Data.Env->CubemapIndexBuffer.get());
		}

		m_Data.Env->Framebuffer->Detach();
		m_Data.Env->Framebuffer->Resize(32, 32);
		m_Data.Env->Framebuffer->Attach();

		m_Data.Env->IrradianceShader->Attach();
		m_Data.Env->IrradianceShader->SetInt("u_EnvironmentMap", 0);
		m_Data.Env->IrradianceShader->SetMat4f("u_Projection", captureProjection);
		m_Data.Env->Environment->Attach(0);

		for (uint8_t i = 0; i < 6; i++)
		{
			m_Data.Env->IrradianceShader->SetMat4f("u_View", captureViews[i]);
			//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Irradiance->GetRendererID());

			// FIX:
			RenderCommand::Clear();
			Renderer::Submit(m_Data.Env->CubemapVertexBuffer.get(), m_Data.Env->CubemapIndexBuffer.get());
		}

		m_Data.Env->Framebuffer->Detach();

		RenderCommand::SetCullFace(RenderCullMode::Default);
		RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
	}
}
