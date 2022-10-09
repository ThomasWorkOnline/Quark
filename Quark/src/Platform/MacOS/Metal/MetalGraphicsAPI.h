#pragma once

#include "Quark/Renderer/GraphicsAPI.h"

namespace Quark {

	class MetalGraphicsAPI final : public GraphicsAPI
	{
	public:
		virtual void                         Init() override {}

		virtual RHIVersion                   GetRHIVersion() const final override { return RHIVersion{}; }

		virtual Scope<CommandBuffer>         CreateCommandBuffer() final override { return nullptr; }
									           
		virtual Scope<Pipeline>              CreatePipeline(const PipelineSpecification& spec) final override { return nullptr; }
		virtual Scope<RenderPass>            CreateRenderPass(const RenderPassSpecification& spec) final override { return nullptr; }
									         
		virtual Scope<VertexBuffer>          CreateVertexBuffer(const void* vertices, size_t size) final override { return nullptr; }
		virtual Scope<VertexBuffer>          CreateVertexBuffer(size_t size) final override { return nullptr; }
									         
		virtual Scope<IndexBuffer>           CreateIndexBuffer(const uint32_t* indices, uint32_t count) final override { return nullptr; }
		virtual Scope<IndexBuffer>           CreateIndexBuffer(uint32_t count) final override { return nullptr; }
									       
		virtual Scope<Cubemap>               CreateCubemap(const CubemapSpecification& spec) final override { return nullptr; }
									       
		virtual Scope<Font>                  CreateFont(std::string_view filepath, uint32_t fontSize) final override { return nullptr; }
									       
		virtual Scope<Framebuffer>           CreateFramebuffer(const FramebufferSpecification& spec) final override { return nullptr; }
		virtual Scope<FramebufferAttachment> CreateFramebufferAttachment(const FramebufferAttachmentSpecification& spec) final override { return nullptr; }
		
		virtual Scope<Shader>                CreateShader(std::string_view filepath) final override { return nullptr; }
		virtual Scope<Shader>                CreateShader(std::string_view name, SpirvView vertexSource, SpirvView fragmentSource) final override { return nullptr; }
		virtual Scope<Shader>                CreateShader(std::string_view name, SpirvView vertexSource, SpirvView geometrySource, SpirvView fragmentSource) final override { return nullptr; }

		virtual Scope<Shader>                CreateShaderLegacy(std::string_view filepath) final override { return nullptr; }
		virtual Scope<Shader>                CreateShaderLegacy(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource) final override { return nullptr; }
		virtual Scope<Shader>                CreateShaderLegacy(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource) final override { return nullptr; }
									       
		virtual Scope<Sampler>               CreateSampler(const SamplerSpecification& spec) final override { return nullptr; }

		virtual Scope<Texture2D>             CreateTexture2D(const Texture2DSpecification& spec) final override { return nullptr; }
		virtual Scope<Texture2D>             CreateTexture2D(std::string_view filepath) final override { return nullptr; }
									       
		virtual Scope<Texture2DArray>        CreateTexture2DArray(const Texture2DArraySpecification& spec) final override { return nullptr; }
									       
		virtual Scope<UniformBuffer>         CreateUniformBuffer(const UniformBufferSpecification& spec) final override { return nullptr; }
									       
		virtual const char*                  GetName() const final override { return "Metal"; }
		virtual std::string                  GetSpecification() const final override { return std::string(); }
	};
}
