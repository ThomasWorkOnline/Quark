#pragma once

#include "Quark/Renderer/GraphicsAPI.h"

namespace Quark {

	class Direct3DGraphicsAPI final : public GraphicsAPI
	{
	public:
		virtual void                         Init() final override;

		virtual RHIVersion                   GetRHIVersion() const final override;

		virtual Scope<CommandBuffer>         CreateCommandBuffer() final override;

		virtual Scope<Pipeline>              CreatePipeline(const PipelineSpecification& spec) final override;
		virtual Scope<RenderPass>            CreateRenderPass(const RenderPassSpecification& spec) final override;

		virtual Scope<VertexBuffer>          CreateVertexBuffer(const void* vertices, size_t size) final override;
		virtual Scope<VertexBuffer>          CreateVertexBuffer(size_t size) final override;

		virtual Scope<IndexBuffer>           CreateIndexBuffer(const void* indices, uint32_t count, IndexType indexType) final override;
		virtual Scope<IndexBuffer>           CreateIndexBuffer(uint32_t count, IndexType indexType) final override;

		virtual Scope<Cubemap>               CreateCubemap(const CubemapSpecification& spec) final override;

		virtual Scope<Font>                  CreateFont(std::string_view filepath, uint32_t fontSize) final override;

		virtual Scope<Framebuffer>           CreateFramebuffer(const FramebufferSpecification& spec) final override;
		virtual Scope<FramebufferAttachment> CreateFramebufferAttachment(const FramebufferAttachmentSpecification& spec) final override;

		virtual Scope<Shader>                CreateShader(std::string_view filepath) final override;

		virtual Scope<Shader>                CreateShader(std::string_view name,
		                                                  std::string_view vertexSource,
		                                                  std::string_view fragmentSource) final override;

		virtual Scope<Shader>                CreateShader(std::string_view name,
		                                                  std::string_view vertexSource,
		                                                  std::string_view geometrySource,
		                                                  std::string_view fragmentSource) final override;

		virtual Scope<Shader>                CreateShader(std::string_view name,
		                                                  std::span<const uint32_t> vertexSpirv,
		                                                  std::span<const uint32_t> fragmentSpirv) final override;

		virtual Scope<Shader>                CreateShader(std::string_view name,
		                                                  std::span<const uint32_t> vertexSpirv,
		                                                  std::span<const uint32_t> geometrySpirv,
		                                                  std::span<const uint32_t> fragmentSpirv) final override;

		virtual Scope<Sampler>               CreateSampler(const SamplerSpecification& spec) final override;

		virtual Scope<Texture2D>             CreateTexture2D(const Texture2DSpecification& spec) final override;
		virtual Scope<Texture2D>             CreateTexture2D(std::string_view filepath) final override;

		virtual Scope<Texture2DArray>        CreateTexture2DArray(const Texture2DArraySpecification& spec) final override;

		virtual Scope<UniformBuffer>         CreateUniformBuffer(size_t size) final override;

		virtual const char*                  GetName() const final override { return "Direct3D"; }
		virtual const char*                  GetDeviceName() const final override;
		virtual std::string                  GetSpecification() const final override;
	};
}
