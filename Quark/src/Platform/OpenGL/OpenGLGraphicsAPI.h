#pragma once

#include "Quark/Renderer/GraphicsAPI.h"

namespace Quark {

	class OpenGLGraphicsAPI final : public GraphicsAPI
	{
	public:
		virtual void                         Init() override;
									       
		virtual                              Version GetVersion() const override;
									         
		virtual void                         SetClearColor(const Vec4f& rgba) override;
		virtual void                         SetCullFace(RenderCullMode face) override;
		virtual void                         SetDepthFunction(RenderDepthFunction func) override;
									         
		virtual void                         SetLineThickness(float thickness) override;
		virtual float                        GetLineThickness() const override;
									         
		virtual Scope<CommandBuffer>         CreateCommandBuffer() override;
								       
		virtual Scope<Pipeline>              CreatePipeline(const PipelineSpecification& spec) override;
		virtual Scope<RenderPass>            CreateRenderPass(const RenderPassSpecification& spec) override;
								       
		virtual Scope<VertexBuffer>          CreateVertexBuffer(const void* vertices, size_t size) override;
		virtual Scope<VertexBuffer>          CreateVertexBuffer(size_t size) override;
								       
		virtual Scope<IndexBuffer>           CreateIndexBuffer(const uint32_t* indices, uint32_t count) override;
		virtual Scope<IndexBuffer>           CreateIndexBuffer(uint32_t count) override;
								       
		virtual Scope<Cubemap>               CreateCubemap(const CubemapSpecification& spec) override;
								       
		virtual Scope<Font>                  CreateFont(std::string_view filepath, uint32_t fontSize) override;
								       
		virtual Scope<Framebuffer>           CreateFramebuffer(const FramebufferSpecification& spec) override;
		virtual Scope<FramebufferAttachment> CreateFramebufferAttachment(const FramebufferAttachmentSpecification& spec) override;

		virtual Scope<Shader>                CreateShader(std::string_view filepath) override;
		virtual Scope<Shader>                CreateShader(std::string_view name, SpirvSource vertexSource, SpirvSource fragmentSource) override;
		virtual Scope<Shader>                CreateShader(std::string_view name, SpirvSource vertexSource, SpirvSource geometrySource, SpirvSource fragmentSource) override;
								       
		virtual Scope<Texture2D>             CreateTexture2D(const Texture2DSpecification& spec) override;
		virtual Scope<Texture2D>             CreateTexture2D(std::string_view filepath, const TextureRenderModes& renderModes = {}) override;
								       
		virtual Scope<Texture2DArray>        CreateTexture2DArray(const Texture2DArraySpecification& spec) override;
									       
		virtual Scope<UniformBuffer>         CreateUniformBuffer(const UniformBufferSpecification& spec) override;
									       
		virtual const char*                  GetName() const override { return "OpenGL"; }
		virtual std::string                  GetSpecification() const override;
	};
}
