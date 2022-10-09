#include "qkpch.h"
#include "Direct3DGraphicsAPI.h"

namespace Quark {

	void Direct3DGraphicsAPI::Init()
	{
	}

	RHIVersion Direct3DGraphicsAPI::GetRHIVersion() const
	{
		return RHIVersion();
	}

	Scope<CommandBuffer> Direct3DGraphicsAPI::CreateCommandBuffer()
	{
		return Scope<CommandBuffer>();
	}

	Scope<Pipeline> Direct3DGraphicsAPI::CreatePipeline(const PipelineSpecification& spec)
	{
		return Scope<Pipeline>();
	}

	Scope<RenderPass> Direct3DGraphicsAPI::CreateRenderPass(const RenderPassSpecification& spec)
	{
		return Scope<RenderPass>();
	}

	Scope<VertexBuffer> Direct3DGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return Scope<VertexBuffer>();
	}

	Scope<VertexBuffer> Direct3DGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return Scope<VertexBuffer>();
	}

	Scope<IndexBuffer> Direct3DGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return Scope<IndexBuffer>();
	}

	Scope<IndexBuffer> Direct3DGraphicsAPI::CreateIndexBuffer(uint32_t count)
	{
		return Scope<IndexBuffer>();
	}

	Scope<Cubemap> Direct3DGraphicsAPI::CreateCubemap(const CubemapSpecification& spec)
	{
		return Scope<Cubemap>();
	}

	Scope<Font> Direct3DGraphicsAPI::CreateFont(std::string_view filepath, uint32_t fontSize)
	{
		return Scope<Font>();
	}

	Scope<Framebuffer> Direct3DGraphicsAPI::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return Scope<Framebuffer>();
	}

	Scope<FramebufferAttachment> Direct3DGraphicsAPI::CreateFramebufferAttachment(const FramebufferAttachmentSpecification& spec)
	{
		return Scope<FramebufferAttachment>();
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShader(std::string_view filepath)
	{
		return Scope<Shader>();
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShader(std::string_view name, SpirvView vertexSource, SpirvView fragmentSource)
	{
		return Scope<Shader>();
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShader(std::string_view name, SpirvView vertexSource, SpirvView geometrySource, SpirvView fragmentSource)
	{
		return Scope<Shader>();
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShaderLegacy(std::string_view filepath)
	{
		return Scope<Shader>();
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShaderLegacy(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return Scope<Shader>();
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShaderLegacy(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return Scope<Shader>();
	}

	Scope<Sampler> Direct3DGraphicsAPI::CreateSampler(const SamplerSpecification& spec)
	{
		return Scope<Sampler>();
	}

	Scope<Texture2D> Direct3DGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return Scope<Texture2D>();
	}

	Scope<Texture2D> Direct3DGraphicsAPI::CreateTexture2D(std::string_view filepath)
	{
		return Scope<Texture2D>();
	}

	Scope<Texture2DArray> Direct3DGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return Scope<Texture2DArray>();
	}

	Scope<UniformBuffer> Direct3DGraphicsAPI::CreateUniformBuffer(const UniformBufferSpecification& spec)
	{
		return Scope<UniformBuffer>();
	}

	const char* Direct3DGraphicsAPI::GetName() const
	{
		return "Direct3D";
	}

	std::string Direct3DGraphicsAPI::GetSpecification() const
	{
		return {};
	}
}
