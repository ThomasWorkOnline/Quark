#include "qkpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

#include "UniformBuffer.h"

namespace Quark {

	ShaderLibrary Renderer::s_ShaderLibrary;

	struct RendererData
	{
		uint32_t MaxUniformBuffers = 0;

		Ref<Texture2D> DefaultTexture;

		// Ensure std140 layout
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};

		CameraData CameraBufferData{};
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static RendererData* s_Data;

	void Renderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		RenderCommand::Init();
		RenderCommand::SetClearColor({ 0.01f, 0.01f, 0.01f, 1.0f });
		QK_CORE_INFO(RenderCommand::GetSpecification());

		s_Data = new RendererData();

		Renderer2D::Initialize();

		uint32_t textureColor = 0xffffffff;
		Texture2DSpecification spec = { 1, 1, 1,
			TextureDataFormat::RGBA,
			TextureInternalFormat::RGBA8,
			TextureFilteringMode::Nearest, TextureFilteringMode::Nearest, TextureTilingMode::Repeat
		};

		s_Data->DefaultTexture = Texture2D::Create(spec);
		s_Data->DefaultTexture->SetData(&textureColor, sizeof(uint32_t));

		s_Data->MaxUniformBuffers = RenderCommand::GetHardwareConstraints().UniformBufferConstraints.MaxBindings;
		s_Data->CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();

		Renderer2D::Dispose();
		delete s_Data;
	}

	void Renderer::BeginScene(const Camera& sceneCamera, const Transform3DComponent& cameraTransform)
	{
		glm::mat4 rotate = glm::toMat4(cameraTransform.Orientation);
		glm::mat4 view = glm::translate(rotate, (glm::vec3)-cameraTransform.Position);

		BeginScene(sceneCamera.GetProjection(), view);
	}

	void Renderer::BeginScene(const glm::mat4& cameraProjection, const glm::mat4& cameraView)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->CameraBufferData.ViewProjection = cameraProjection * cameraView;
		s_Data->CameraUniformBuffer->Attach();
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBufferData, sizeof(RendererData::CameraData));

		Renderer2D::BeginScene();
	}

	void Renderer::EndScene()
	{
		QK_ASSERT_RENDER_THREAD();

		Renderer2D::EndScene();
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->DefaultTexture->Attach();

		shader->Attach();
		shader->SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(va);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<Texture2D>& texture, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		texture->Attach();

		shader->Attach();
		shader->SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(va);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<Framebuffer>& framebuffer, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		framebuffer->AttachColorAttachment(0);

		shader->Attach();
		shader->SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(va);
	}

	void Renderer::OnWindowResized(uint32_t width, uint32_t height)
	{
		QK_ASSERT_RENDER_THREAD();
		RenderCommand::SetViewport(0, 0, width, height);
	}
}
