#include "qkpch.h"
#include "Direct3DGraphicsAPI.h"

namespace Quark {

	void Direct3DGraphicsAPI::Init()
	{
	}

	RHIVersion Direct3DGraphicsAPI::GetRHIVersion() const
	{
		return {};
	}

	Scope<CommandBuffer> Direct3DGraphicsAPI::CreateCommandBuffer()
	{
		return nullptr;
	}

	Scope<Pipeline> Direct3DGraphicsAPI::CreatePipeline(const PipelineSpecification& spec)
	{
		return nullptr;
	}

	Scope<RenderPass> Direct3DGraphicsAPI::CreateRenderPass(const RenderPassSpecification& spec)
	{
		return nullptr;
	}

	Scope<VertexBuffer> Direct3DGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return nullptr;
	}

	Scope<VertexBuffer> Direct3DGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return nullptr;
	}

	Scope<IndexBuffer> Direct3DGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return nullptr;
	}

	Scope<IndexBuffer> Direct3DGraphicsAPI::CreateIndexBuffer(uint32_t count)
	{
		return nullptr;
	}

	Scope<Cubemap> Direct3DGraphicsAPI::CreateCubemap(const CubemapSpecification& spec)
	{
		return nullptr;
	}

	Scope<Font> Direct3DGraphicsAPI::CreateFont(std::string_view filepath, uint32_t fontSize)
	{
		return nullptr;
	}

	Scope<Framebuffer> Direct3DGraphicsAPI::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return nullptr;
	}

	Scope<FramebufferAttachment> Direct3DGraphicsAPI::CreateFramebufferAttachment(const FramebufferAttachmentSpecification& spec)
	{
		return nullptr;
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShader(std::string_view filepath)
	{
		return nullptr;
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShader(
		std::string_view name,
		std::string_view vertexSource,
		std::string_view fragmentSource)
	{
		return nullptr;
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShader(std::string_view name,
		std::string_view vertexSource,
		std::string_view geometrySource,
		std::string_view fragmentSource)
	{
		return nullptr;
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShader(
		std::string_view name,
		std::span<const uint32_t> vertexSpirv,
		std::span<const uint32_t> fragmentSpirv)
	{
		return nullptr;
	}

	Scope<Shader> Direct3DGraphicsAPI::CreateShader(
		std::string_view name, std::span<const uint32_t> vertexSpirv,
		std::span<const uint32_t> geometrySpirv,
		std::span<const uint32_t> fragmentSpirv)
	{
		return nullptr;
	}

	Scope<Sampler> Direct3DGraphicsAPI::CreateSampler(const SamplerSpecification& spec)
	{
		return nullptr;
	}

	Scope<Texture2D> Direct3DGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return nullptr;
	}

	Scope<Texture2D> Direct3DGraphicsAPI::CreateTexture2D(std::string_view filepath)
	{
		return nullptr;
	}

	Scope<Texture2DArray> Direct3DGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return nullptr;
	}

	Scope<UniformBuffer> Direct3DGraphicsAPI::CreateUniformBuffer(size_t size)
	{
		return nullptr;
	}

	const char* Direct3DGraphicsAPI::GetDeviceName() const
	{
		return "";
	}

	std::string Direct3DGraphicsAPI::GetSpecification() const
	{
		return {};
	}
}
