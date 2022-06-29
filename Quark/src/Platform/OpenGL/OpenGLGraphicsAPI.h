#pragma once

#include "Quark/Renderer/GraphicsAPI.h"

namespace Quark {

	class OpenGLGraphicsAPI final : public GraphicsAPI
	{
	public:
		virtual void                   Init() override;

		virtual                        Version GetVersion() const override;
		virtual const                  HardwareConstraints& GetHardwareConstraints() const override { return m_Constraints; }

		virtual void                   SetClearColor(const Vec4f& rgba) override;
		virtual void                   Clear() override;
		virtual void                   SetCullFace(RenderCullFace face) override;
		virtual void                   SetDepthFunction(RenderDepthFunction func) override;
		virtual void                   SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void                   Draw(uint32_t offset, uint32_t count) override;
		virtual void                   DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void                   DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0) override;
		virtual void                   DrawLines(const Ref<VertexArray>& vertexArray, uint32_t count) override;
		virtual void                   DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		virtual void                   SetLineThickness(float thickness) override;
		virtual float                  GetLineThickness() const override;

		virtual Ref<VertexBuffer>      CreateVertexBuffer(const void* vertices, size_t size) override;
		virtual Ref<VertexBuffer>      CreateVertexBuffer(size_t size) override;
								       
		virtual Ref<IndexBuffer>       CreateIndexBuffer(const uint32_t* indices, uint32_t count) override;
		virtual Ref<IndexBuffer>       CreateIndexBuffer(uint32_t count) override;
								       
		virtual Ref<Cubemap>           CreateCubemap(const CubemapSpecification& spec) override;
								       
		virtual Ref<Font>              CreateFont(std::string_view filepath, uint32_t fontSize) override;
								       
		virtual Ref<Framebuffer>       CreateFramebuffer(const FramebufferSpecification& spec) override;

		virtual Scope<RenderPipeline>  CreateRenderPipeline() override;
								       
		virtual Ref<Shader>            CreateShader(std::string_view filepath) override;
		virtual Ref<Shader>            CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource) override;
		virtual Ref<Shader>            CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource) override;
									   
		virtual Ref<Texture2D>         CreateTexture2D(const Texture2DSpecification& spec) override;
		virtual Ref<Texture2D>         CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor = {}) override;
									   
		virtual Ref<Texture2DArray>    CreateTexture2DArray(const Texture2DArraySpecification& spec) override;
									   
		virtual Ref<UniformBuffer>     CreateUniformBuffer(size_t size, uint32_t binding) override;
									   
		virtual Ref<VertexArray>       CreateVertexArray() override;
									   
		virtual const char*            GetName() const override { return "OpenGL"; }
		virtual std::string            GetSpecification() const override;

	private:
		HardwareConstraints m_Constraints{};
	};
}
