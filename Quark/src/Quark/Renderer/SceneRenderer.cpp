#include "qkpch.h"
#include "SceneRenderer.h"

#include "Quark/Core/Application.h"
#include "Renderer.h"
#include "RenderCommand.h"

namespace Quark {

	struct Vertex2D
	{
		glm::vec3 Position;
		glm::vec3 Color;
	};

	static constexpr Vertex2D s_Vertices[] = {
		{ { -0.5f, -0.5f, 0.0f }, { 1.0f,  0.0f,  0.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f,  1.0f,  0.0f } },
		{ {  0.5f,  0.5f, 0.0f }, { 0.0f,  0.0f,  1.0f } },
		{ { -0.5f,  0.5f, 0.0f }, { 1.0f,  1.0f,  1.0f } }
	};

	static constexpr uint32_t s_Indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	static BufferLayout s_Vertex2DLayout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float3, "a_Color"    }
	};

	SceneRenderer::SceneRenderer()
	{
		auto& window = Application::Get().GetWindow();
		m_ViewportSize = { window.GetWidth(), window.GetHeight() };

		Initialize();
	}

	SceneRenderer::SceneRenderer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
		auto& window = Application::Get().GetWindow();
		m_ViewportSize = { window.GetWidth(), window.GetHeight() };

		Initialize();

		auto view = m_Scene->m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			cameraComponent.Camera.Resize(m_ViewportSize.x, m_ViewportSize.y);
		}
	}

	SceneRenderer::~SceneRenderer()
	{
		// Internally waits for all operations to be completed
		m_Data.GraphicsPipeline.reset();
	}

	void SceneRenderer::SetContext(const Ref<Scene>& scene)
	{
		m_Scene = scene;

		auto view = m_Scene->m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			cameraComponent.Camera.Resize(m_ViewportSize.x, m_ViewportSize.y);
		}
	}

	void SceneRenderer::SetEnvironment(std::string_view filepath)
	{
		NewEnvironment(filepath);
	}

	void SceneRenderer::OnRender()
	{
		m_Data.GraphicsPipeline->BeginFrame();

		if (m_Scene && m_Scene->m_CameraEntity)
		{
			auto& sceneCamera = m_Scene->m_CameraEntity.GetComponent<CameraComponent>().Camera;
			auto& cameraTransform = m_Scene->m_CameraEntity.GetComponent<Transform3DComponent>();

			Mat4f rotate = glm::toMat4(cameraTransform.Orientation);
			Mat4f view = glm::translate(rotate, (Vec3f)-cameraTransform.Position);

			m_Data.CameraBufferData.View = view;
			m_Data.CameraBufferData.Projection = sceneCamera.GetProjection();

			m_Data.GraphicsPipeline->GetUniformBuffer()->SetData(&m_Data.CameraBufferData, sizeof(SceneData::CameraUniformBufferData));

			// Environment map
			if (m_Data.Env)
			{
				RenderCommand::SetCullFace(RenderCullMode::Front);
				RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

				m_Data.Env->SkyboxShader->Attach();
				m_Data.Env->SkyboxShader->SetMat4("u_View", view);
				m_Data.Env->SkyboxShader->SetMat4("u_Projection", sceneCamera.GetProjection());
				m_Data.Env->SkyboxShader->SetInt("u_EnvironmentMap", 0);
				m_Data.Env->SkyboxShader->SetFloat("u_Exposure", 1.0f);

				m_Data.Env->Environment->Attach(0);
				m_Data.Env->Irradiance->Attach(5); // TODO: put inside scene uniform buffer

				RenderCommand::DrawIndexed(m_Data.Env->CubemapIndexBuffer->GetCount());
				RenderCommand::SetCullFace(RenderCullMode::Default);
				RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
			}
		}

		GeometryPass();

		m_Data.GraphicsPipeline->EndFrame();
		m_Data.GraphicsPipeline->Submit();
	}

	void SceneRenderer::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_ViewportSize = { viewportWidth, viewportHeight };
		m_Data.GraphicsPipeline->Resize(m_ViewportSize.x, m_ViewportSize.y);

		auto view = m_Scene->m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			cameraComponent.Camera.Resize(m_ViewportSize.x, m_ViewportSize.y);
		}
	}

	void SceneRenderer::GeometryPass()
	{
		m_Data.GraphicsPipeline->BeginRenderPass(m_Data.GeometryPass);

		m_Data.GraphicsPipeline->GetCommandBuffer()->BindVertexBuffer(m_Data.VertexBuffer, 0);
		m_Data.GraphicsPipeline->GetCommandBuffer()->BindIndexBuffer(m_Data.IndexBuffer);

		m_Data.GraphicsPipeline->GetCommandBuffer()->DrawIndexed(sizeof(s_Indices) / sizeof(uint32_t));
		m_Data.GraphicsPipeline->EndRenderPass();
	}

	void SceneRenderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		m_Data.VertexShader = Shader::Create("../Quark/assets/shaders/version/4.50/bin/vert.spv");
		m_Data.FragmentShader = Shader::Create("../Quark/assets/shaders/version/4.50/bin/frag.spv");

		{
			RenderPassSpecification spec;
			spec.BindPoint = PipelineBindPoint::Graphics;
			m_Data.GeometryPass = RenderPass::Create(spec);
		}

		{
			PipelineSpecification spec;
			spec.ViewportWidth = m_ViewportSize.x;
			spec.ViewportHeight = m_ViewportSize.y;
			spec.FramesInFlight = Renderer::FramesInFlight;
			spec.CameraUniformBufferSize = sizeof(SceneData::CameraUniformBufferData);
			spec.Layout = s_Vertex2DLayout;
			spec.RenderPass = m_Data.GeometryPass;
			spec.VertexShader = m_Data.VertexShader;
			spec.FragmentShader = m_Data.FragmentShader;

			m_Data.GraphicsPipeline = Pipeline::Create(spec);
		}

		{
			m_Data.VertexBuffer = VertexBuffer::Create(s_Vertices, sizeof(s_Vertices));
			m_Data.IndexBuffer = IndexBuffer::Create(s_Indices, sizeof(s_Indices) / sizeof(uint32_t));
		}
	}

	void SceneRenderer::NewEnvironment(std::string_view environmentFilepath)
	{
		QK_PROFILE_FUNCTION();

		if (!m_Data.Env)
		{
			auto& assetDir = Application::Get().GetOptions().AssetDir;

			m_Data.Env = CreateScope<EnvironmentData>();
			m_Data.Env->SkyboxShader = Shader::Create(assetDir + "/assets/shaders/version/3.30/cubemap.glsl");
			m_Data.Env->IrradianceShader = Shader::Create(assetDir + "/assets/shaders/version/3.30/irradiance.glsl");
			m_Data.Env->EquirectangleToCubemapShader = Shader::Create(assetDir + "/assets/shaders/version/3.30/equirectangle_to_cubemap.glsl");

			{
				Mesh cube = Mesh::GenerateUnitCube();
				m_Data.Env->CubemapVertexBuffer = cube.GetVertexBuffer();
				m_Data.Env->CubemapIndexBuffer = cube.GetIndexBuffer();
			}

			{
				CubemapSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				m_Data.Env->Environment = Cubemap::Create(spec);

				spec.Width = 32;
				spec.Height = 32;
				m_Data.Env->Irradiance = Cubemap::Create(spec);
			}

			{
				FramebufferSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				spec.Attachments = { { FramebufferTargetAttachment::ColorAttachment0, ColorDataFormat::RGB, InternalColorFormat::RGBA16f } };
				m_Data.Env->Framebuffer = Framebuffer::Create(spec);
			}
		}

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		auto hdrTexture = Texture2D::Create(environmentFilepath);
		hdrTexture->Attach(0);

		m_Data.Env->Framebuffer->Attach();

		m_Data.Env->EquirectangleToCubemapShader->Attach();
		m_Data.Env->EquirectangleToCubemapShader->SetInt("u_EquirectangularMap", 0);
		m_Data.Env->EquirectangleToCubemapShader->SetMat4("u_Projection", captureProjection);

		RenderCommand::SetCullFace(RenderCullMode::Front);
		RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

		for (uint8_t i = 0; i < 6; i++)
		{
			m_Data.Env->EquirectangleToCubemapShader->SetMat4("u_View", captureViews[i]);
			//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Environment->GetRendererID());

			// FIX:
			RenderCommand::Clear();
			RenderCommand::DrawIndexed(m_Data.Env->CubemapIndexBuffer->GetCount());
		}

		m_Data.Env->Framebuffer->Detach();
		m_Data.Env->Framebuffer->Resize(32, 32);
		m_Data.Env->Framebuffer->Attach();

		m_Data.Env->IrradianceShader->Attach();
		m_Data.Env->IrradianceShader->SetInt("u_EnvironmentMap", 0);
		m_Data.Env->IrradianceShader->SetMat4("u_Projection", captureProjection);
		m_Data.Env->Environment->Attach(0);

		for (uint8_t i = 0; i < 6; i++)
		{
			m_Data.Env->IrradianceShader->SetMat4("u_View", captureViews[i]);
			//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Irradiance->GetRendererID());

			// FIX:
			RenderCommand::Clear();
			RenderCommand::DrawIndexed(m_Data.Env->CubemapIndexBuffer->GetCount());
		}

		m_Data.Env->Framebuffer->Detach();

		RenderCommand::SetCullFace(RenderCullMode::Default);
		RenderCommand::SetDepthFunction(RenderDepthFunction::Default);

		RenderCommand::SetViewport(0, 0, m_ViewportSize.x, m_ViewportSize.y);
	}
}
