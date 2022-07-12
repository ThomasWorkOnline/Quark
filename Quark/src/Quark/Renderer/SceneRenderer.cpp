#include "qkpch.h"
#include "SceneRenderer.h"

#include "Quark/Core/Application.h"
#include "Quark/Filesystem/Filesystem.h"

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

	struct EnvironmentData
	{
		Ref<Shader> SkyboxShader;
		Ref<Shader> IrradianceShader;
		Ref<Shader> EquirectangleToCubemapShader;
		Ref<VertexBuffer> CubemapVertexBuffer;
		Ref<IndexBuffer> CubemapIndexBuffer;
		Ref<Framebuffer> Framebuffer;

		Ref<Cubemap> Environment;
		Ref<Cubemap> Irradiance;
	};

	struct SceneData
	{
		// Assure std140 layout
		struct CameraUniformBufferData
		{
			Mat4f View = Mat4(1.0f);
			Mat4f Projection = Mat4(1.0f);
		};

		Scope<EnvironmentData> Env;
		CameraUniformBufferData CameraBufferData;

		Ref<Shader> VertexShader;
		Ref<Shader> FragmentShader;

		Ref<Pipeline> GraphicsPipeline;
		Ref<RenderPass> GeometryPass;

		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer> IndexBuffer;
	};

	static Scope<SceneData> s_Data;

	Ref<Scene> SceneRenderer::s_Scene;
	Vec2i SceneRenderer::s_ViewportSize{};

	void SceneRenderer::SetContext(const Ref<Scene>& scene)
	{
		s_Scene = scene;

		auto view = s_Scene->m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			cameraComponent.Camera.Resize(s_ViewportSize.x, s_ViewportSize.y);
		}
	}

	void SceneRenderer::SetEnvironment(std::string_view filepath)
	{
		NewEnvironment(filepath);
	}

	void SceneRenderer::OnRender()
	{
		s_Data->GraphicsPipeline->BeginFrame();

		if (s_Scene && s_Scene->m_CameraEntity)
		{
			auto& sceneCamera = s_Scene->m_CameraEntity.GetComponent<CameraComponent>().Camera;
			auto& cameraTransform = s_Scene->m_CameraEntity.GetComponent<Transform3DComponent>();

			Mat4f rotate = glm::toMat4(cameraTransform.Orientation);
			Mat4f view = glm::translate(rotate, (Vec3f)-cameraTransform.Position);

			s_Data->CameraBufferData.View = view;
			s_Data->CameraBufferData.Projection = sceneCamera.GetProjection();

			s_Data->GraphicsPipeline->GetUniformBuffer()->SetData(&s_Data->CameraBufferData, sizeof(SceneData::CameraUniformBufferData));

			// Environment map
			if (s_Data->Env)
			{
				RenderCommand::SetCullFace(RenderCullMode::Front);
				RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

				s_Data->Env->SkyboxShader->Attach();
				s_Data->Env->SkyboxShader->SetMat4("u_View", view);
				s_Data->Env->SkyboxShader->SetMat4("u_Projection", sceneCamera.GetProjection());
				s_Data->Env->SkyboxShader->SetInt("u_EnvironmentMap", 0);
				s_Data->Env->SkyboxShader->SetFloat("u_Exposure", 1.0f);

				s_Data->Env->Environment->Attach(0);
				s_Data->Env->Irradiance->Attach(5); // TODO: put inside scene uniform buffer

				RenderCommand::DrawIndexed(s_Data->Env->CubemapIndexBuffer->GetCount());
				RenderCommand::SetCullFace(RenderCullMode::Default);
				RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
			}
		}

		GeometryPass();

		s_Data->GraphicsPipeline->EndFrame();
		s_Data->GraphicsPipeline->Submit();
	}

	void SceneRenderer::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		s_ViewportSize = { viewportWidth, viewportHeight };
		s_Data->GraphicsPipeline->Resize(s_ViewportSize.x, s_ViewportSize.y);

		if (s_Scene)
		{
			auto view = s_Scene->m_Registry.view<CameraComponent>();
			for (auto entity : view)
			{
				auto& cameraComponent = view.get<CameraComponent>(entity);
				cameraComponent.Camera.Resize(s_ViewportSize.x, s_ViewportSize.y);
			}
		}
	}

	void SceneRenderer::GeometryPass()
	{
		s_Data->GraphicsPipeline->BeginRenderPass(s_Data->GeometryPass);

		s_Data->GraphicsPipeline->GetCommandBuffer()->BindVertexBuffer(s_Data->VertexBuffer, 0);
		s_Data->GraphicsPipeline->GetCommandBuffer()->BindIndexBuffer(s_Data->IndexBuffer);

		s_Data->GraphicsPipeline->GetCommandBuffer()->DrawIndexed(sizeof(s_Indices) / sizeof(uint32_t));
		s_Data->GraphicsPipeline->EndRenderPass();
	}

	void SceneRenderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		auto& window = Application::Get().GetWindow();
		s_ViewportSize = { window.GetWidth(), window.GetHeight() };

		s_Data = CreateScope<SceneData>();

		if (GraphicsAPI::GetAPI() == API::Vulkan)
		{
			s_Data->VertexShader = Shader::Create("../Quark/assets/shaders/version/4.50/bin/vert.spv");
			s_Data->FragmentShader = Shader::Create("../Quark/assets/shaders/version/4.50/bin/frag.spv");
		}
		else
		{
			std::string vertexSource = Filesystem::ReadTextFile("../Quark/assets/shaders/version/4.50/shader.vert");
			std::string fragmentSource = Filesystem::ReadTextFile("../Quark/assets/shaders/version/4.50/shader.frag");

			s_Data->VertexShader = Shader::Create("shader", vertexSource, fragmentSource);
		}

		{
			RenderPassSpecification spec;
			spec.BindPoint = PipelineBindPoint::Graphics;
			s_Data->GeometryPass = RenderPass::Create(spec);
		}

		{
			PipelineSpecification spec;
			spec.ViewportWidth = s_ViewportSize.x;
			spec.ViewportHeight = s_ViewportSize.y;
			spec.FramesInFlight = Renderer::FramesInFlight;
			spec.CameraUniformBufferSize = sizeof(SceneData::CameraUniformBufferData);
			spec.Layout = s_Vertex2DLayout;
			spec.RenderPass = s_Data->GeometryPass;
			spec.VertexShader = s_Data->VertexShader;
			spec.FragmentShader = s_Data->FragmentShader;

			s_Data->GraphicsPipeline = Pipeline::Create(spec);
		}

		{
			s_Data->VertexBuffer = VertexBuffer::Create(s_Vertices, sizeof(s_Vertices));
			s_Data->IndexBuffer = IndexBuffer::Create(s_Indices, sizeof(s_Indices) / sizeof(uint32_t));
		}
	}

	void SceneRenderer::Shutdown()
	{
		// Internally waits for all operations to be completed
		s_Data->GraphicsPipeline.reset();
		s_Data.reset();
	}

	void SceneRenderer::NewEnvironment(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		if (!s_Data->Env)
		{
			auto& assetDir = Application::Get().GetOptions().AssetDir;

			s_Data->Env = CreateScope<EnvironmentData>();
			s_Data->Env->SkyboxShader = Shader::Create(assetDir + "/assets/shaders/version/3.30/cubemap.glsl");
			s_Data->Env->IrradianceShader = Shader::Create(assetDir + "/assets/shaders/version/3.30/irradiance.glsl");
			s_Data->Env->EquirectangleToCubemapShader = Shader::Create(assetDir + "/assets/shaders/version/3.30/equirectangle_to_cubemap.glsl");

			{
				Mesh cube = Mesh::GenerateUnitCube();
				s_Data->Env->CubemapVertexBuffer = cube.GetVertexBuffer();
				s_Data->Env->CubemapIndexBuffer = cube.GetIndexBuffer();
			}

			{
				CubemapSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				s_Data->Env->Environment = Cubemap::Create(spec);

				spec.Width = 32;
				spec.Height = 32;
				s_Data->Env->Irradiance = Cubemap::Create(spec);
			}

			{
				FramebufferSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				spec.Attachments = { { FramebufferTargetAttachment::ColorAttachment0, ColorDataFormat::RGB, InternalColorFormat::RGBA16f } };
				s_Data->Env->Framebuffer = Framebuffer::Create(spec);
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

		auto hdrTexture = Texture2D::Create(filepath);
		hdrTexture->Attach(0);

		s_Data->Env->Framebuffer->Attach();
		s_Data->Env->EquirectangleToCubemapShader->Attach();
		s_Data->Env->EquirectangleToCubemapShader->SetInt("u_EquirectangularMap", 0);
		s_Data->Env->EquirectangleToCubemapShader->SetMat4("u_Projection", captureProjection);

		RenderCommand::SetCullFace(RenderCullMode::Front);
		RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

		for (uint8_t i = 0; i < 6; i++)
		{
			s_Data->Env->EquirectangleToCubemapShader->SetMat4("u_View", captureViews[i]);
			//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Environment->GetRendererID());

			// FIX:
			RenderCommand::Clear();
			RenderCommand::DrawIndexed(s_Data->Env->CubemapIndexBuffer->GetCount());
		}

		s_Data->Env->Framebuffer->Detach();
		s_Data->Env->Framebuffer->Resize(32, 32);
		s_Data->Env->Framebuffer->Attach();

		s_Data->Env->IrradianceShader->Attach();
		s_Data->Env->IrradianceShader->SetInt("u_EnvironmentMap", 0);
		s_Data->Env->IrradianceShader->SetMat4("u_Projection", captureProjection);
		s_Data->Env->Environment->Attach(0);

		for (uint8_t i = 0; i < 6; i++)
		{
			s_Data->Env->IrradianceShader->SetMat4("u_View", captureViews[i]);
			//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Irradiance->GetRendererID());

			// FIX:
			RenderCommand::Clear();
			RenderCommand::DrawIndexed(s_Data->Env->CubemapIndexBuffer->GetCount());
		}

		s_Data->Env->Framebuffer->Detach();

		RenderCommand::SetCullFace(RenderCullMode::Default);
		RenderCommand::SetDepthFunction(RenderDepthFunction::Default);

		RenderCommand::SetViewport(0, 0, s_ViewportSize.x, s_ViewportSize.y);
	}
}
