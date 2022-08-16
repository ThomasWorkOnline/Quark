#pragma once

#include "Quark/Renderer/GraphicsAPI.h"

namespace Quark {

	class MetalGraphicsAPI final : public GraphicsAPI
	{
	public:
		virtual void                         Init() override {}
									         
		virtual                              Version GetVersion() const override { return Version{}; }
									         
		virtual void                         SetClearColor(const Vec4f& rgba) override {}
		virtual void                         SetCullFace(RenderCullMode face) override {}
		virtual void                         SetDepthFunction(RenderDepthFunction func) override {}
									         
		virtual void                         SetLineThickness(float thickness) override {}
		virtual float                        GetLineThickness() const override { return 0.0f; }
									       
		virtual Scope<CommandBuffer>         CreateCommandBuffer() override { return nullptr; }
								           
		virtual Scope<Pipeline>              CreatePipeline(const PipelineSpecification& spec) override { return nullptr; }
		virtual Scope<RenderPass>            CreateRenderPass(const RenderPassSpecification& spec) override { return nullptr; }
								           
		virtual Scope<VertexBuffer>          CreateVertexBuffer(const void* vertices, size_t size) override { return nullptr; }
		virtual Scope<VertexBuffer>          CreateVertexBuffer(size_t size) override { return nullptr; }
								           
		virtual Scope<IndexBuffer>           CreateIndexBuffer(const uint32_t* indices, uint32_t count) override { return nullptr; }
		virtual Scope<IndexBuffer>           CreateIndexBuffer(uint32_t count) override { return nullptr; }
								           
		virtual Scope<Cubemap>               CreateCubemap(const CubemapSpecification& spec) override { return nullptr; }
						  	                    
		virtual Scope<Font>                  CreateFont(std::string_view filepath, uint32_t fontSize) override { return nullptr; }
								           
		virtual Scope<Framebuffer>           CreateFramebuffer(const FramebufferSpecification& spec) override { return nullptr; }
		virtual Scope<FramebufferAttachment> CreateFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec) override { return nullptr; }
								           
		virtual Scope<Shader>                CreateShader(std::string_view filepath) override { return nullptr; }
		virtual Scope<Shader>                CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource) override { return nullptr; }
		virtual Scope<Shader>                CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource) override { return nullptr; }
								           
		virtual Scope<Texture2D>             CreateTexture2D(const Texture2DSpecification& spec) override { return nullptr; }
		virtual Scope<Texture2D>             CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor = {}) override { return nullptr; }
								           
		virtual Scope<Texture2DArray>        CreateTexture2DArray(const Texture2DArraySpecification& spec) override { return nullptr; }
								           
		virtual Scope<UniformBuffer>         CreateUniformBuffer(size_t size, uint32_t binding) override { return nullptr; }
									       
		virtual const char*                  GetName() const override { return "Metal"; }
		virtual std::string                  GetSpecification() const override { return std::string(); }
	};
}
