#pragma once

#include "Quark/Renderer/GraphicsAPI.h"

namespace Quark {

	class OpenGLGraphicsAPI final : public GraphicsAPI
	{
	public:
		virtual void                       Init() override;
									       
		virtual                            Version GetVersion() const override;
									       
		virtual void                       SetClearColor(const Vec4f& rgba) override;
		virtual void                       Clear() override;
		virtual void                       SetCullFace(RenderCullMode face) override;
		virtual void                       SetDepthFunction(RenderDepthFunction func) override;
									       
		virtual void                       SetLineThickness(float thickness) override;
		virtual float                      GetLineThickness() const override;
									       
		virtual CommandBuffer*             CreateCommandBuffer() override;
								       
		virtual Pipeline*                  CreatePipeline(const PipelineSpecification& spec) override;
		virtual RenderPass*                CreateRenderPass(const RenderPassSpecification& spec) override;
								       
		virtual VertexBuffer*              CreateVertexBuffer(const void* vertices, size_t size) override;
		virtual VertexBuffer*              CreateVertexBuffer(size_t size) override;
								       
		virtual IndexBuffer*               CreateIndexBuffer(const uint32_t* indices, uint32_t count) override;
		virtual IndexBuffer*               CreateIndexBuffer(uint32_t count) override;
								       
		virtual Cubemap*                   CreateCubemap(const CubemapSpecification& spec) override;
								       
		virtual Font*                      CreateFont(std::string_view filepath, uint32_t fontSize) override;
								       
		virtual Framebuffer*               CreateFramebuffer(const FramebufferSpecification& spec) override;
		virtual FramebufferAttachment*     CreateFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec) override;

		virtual Shader*                    CreateShader(std::string_view filepath) override;
		virtual Shader*                    CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource) override;
		virtual Shader*                    CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource) override;
								       
		virtual Texture2D*                 CreateTexture2D(const Texture2DSpecification& spec) override;
		virtual Texture2D*                 CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor = {}) override;
								       
		virtual Texture2DArray*            CreateTexture2DArray(const Texture2DArraySpecification& spec) override;
									       
		virtual UniformBuffer*             CreateUniformBuffer(size_t size, uint32_t binding) override;
									       
		virtual const char*                GetName() const override { return "OpenGL"; }
		virtual std::string                GetSpecification() const override;
	};
}
