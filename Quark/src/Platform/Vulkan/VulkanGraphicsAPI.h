#pragma once

#include "Quark/Renderer/GraphicsAPI.h"
#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanGraphicsAPI final : public GraphicsAPI
	{
	public:
		virtual void                         Init() final override;
									       
		virtual                              Version GetDriverVersion() const final override;
									         
		virtual Scope<CommandBuffer>         CreateCommandBuffer() final override;
								           
		virtual Scope<Pipeline>              CreatePipeline(const PipelineSpecification& spec) final override;
		virtual Scope<RenderPass>            CreateRenderPass(const RenderPassSpecification& spec) final override;
								           
		virtual Scope<VertexBuffer>          CreateVertexBuffer(const void* vertices, size_t size) final override;
		virtual Scope<VertexBuffer>          CreateVertexBuffer(size_t size) final override;
								           
		virtual Scope<IndexBuffer>           CreateIndexBuffer(const uint32_t* indices, uint32_t count) final override;
		virtual Scope<IndexBuffer>           CreateIndexBuffer(uint32_t count) final override;
								           
		virtual Scope<Cubemap>               CreateCubemap(const CubemapSpecification& spec) final override;
								           
		virtual Scope<Font>                  CreateFont(std::string_view filepath, uint32_t fontSize) final override;
								           
		virtual Scope<Framebuffer>           CreateFramebuffer(const FramebufferSpecification& spec) final override;
		virtual Scope<FramebufferAttachment> CreateFramebufferAttachment(const FramebufferAttachmentSpecification& spec) final override;
									       
		virtual Scope<Shader>                CreateShader(std::string_view filepath) final override;
		virtual Scope<Shader>                CreateShader(std::string_view name, SpirvView vertexSource, SpirvView fragmentSource) final override;
		virtual Scope<Shader>                CreateShader(std::string_view name, SpirvView vertexSource, SpirvView geometrySource, SpirvView fragmentSource) final override;
					
		virtual Scope<Sampler>               CreateSampler(const SamplerSpecification& spec) final override;

		virtual Scope<Texture2D>             CreateTexture2D(const Texture2DSpecification& spec) final override;
		virtual Scope<Texture2D>             CreateTexture2D(std::string_view filepath) final override;
								           
		virtual Scope<Texture2DArray>        CreateTexture2DArray(const Texture2DArraySpecification& spec) final override;
								           
		virtual Scope<UniformBuffer>         CreateUniformBuffer(const UniformBufferSpecification& spec) final override;
									       
		virtual const char*                  GetName() const final override { return "Vulkan"; }
		virtual std::string                  GetSpecification() const final override;
	};
}
