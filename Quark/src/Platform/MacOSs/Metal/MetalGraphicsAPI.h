#pragma once

#include "Quark/Renderer/GraphicsAPI.h"

namespace Quark {

	class MetalGraphicsAPI final : public GraphicsAPI
	{
	public:
		virtual void                       Init() override;
									       
		virtual                            Version GetVersion() const override { return Version{}; }
		virtual const                      HardwareConstraints& GetHardwareConstraints() const override { return m_Constraints; }
									       
		virtual void                       SetClearColor(const Vec4f& rgba) override;
		virtual void                       Clear() override;
		virtual void                       SetCullFace(RenderCullMode face) override {}
		virtual void                       SetDepthFunction(RenderDepthFunction func) override {}
		virtual void                       SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override {}
									       
		virtual void                       Draw(uint32_t offset, uint32_t count) override {}
		virtual void                       DrawIndexed(uint32_t indexCount) override {}
		virtual void                       DrawIndexedInstanced(uint32_t repeatCount, uint32_t indexCount = 0) override {}
		virtual void                       DrawLines(uint32_t vertexCount) override {}
		virtual void                       DrawIndexedLines(uint32_t indexCount = 0) override {}
									       
		virtual void                       SetLineThickness(float thickness) override {}
		virtual float                      GetLineThickness() const override { return 0.0f; }
									       
		virtual Ref<CommandBuffer>         CreateCommandBuffer() override { return nullptr; }
									       
		virtual Ref<Pipeline>              CreatePipeline(const PipelineSpecification& spec) override { return nullptr; }
		virtual Ref<RenderPass>            CreateRenderPass(const RenderPassSpecification& spec) override { return nullptr; }
									       
		virtual Ref<VertexBuffer>          CreateVertexBuffer(const void* vertices, size_t size) override { return nullptr; }
		virtual Ref<VertexBuffer>          CreateVertexBuffer(size_t size) override { return nullptr; }
									       
		virtual Ref<IndexBuffer>           CreateIndexBuffer(const uint32_t* indices, uint32_t count) override { return nullptr; }
		virtual Ref<IndexBuffer>           CreateIndexBuffer(uint32_t count) override { return nullptr; }
									       
		virtual Ref<Cubemap>               CreateCubemap(const CubemapSpecification& spec) override { return nullptr; }
									       
		virtual Ref<Font>                  CreateFont(std::string_view filepath, uint32_t fontSize) override { return nullptr; }
									       
		virtual Ref<Framebuffer>           CreateFramebuffer(const FramebufferSpecification& spec) override { return nullptr; }
		virtual Ref<FramebufferAttachment> CreateFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec) override { return nullptr; }
									       
		virtual Ref<Shader>                CreateShader(std::string_view filepath) override { return nullptr; }
		virtual Ref<Shader>                CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource) override { return nullptr; }
		virtual Ref<Shader>                CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource) override { return nullptr; }
									       
		virtual Ref<Texture2D>             CreateTexture2D(const Texture2DSpecification& spec) override { return nullptr; }
		virtual Ref<Texture2D>             CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor = {}) override { return nullptr; }
									       
		virtual Ref<Texture2DArray>        CreateTexture2DArray(const Texture2DArraySpecification& spec) override { return nullptr; }
									       
		virtual Ref<UniformBuffer>         CreateUniformBuffer(size_t size, uint32_t binding) override { return nullptr; }
									       
		virtual const char*                GetName() const override { return "Metal"; }
		virtual std::string                GetSpecification() const override { return std::string(); }

	private:
		HardwareConstraints m_Constraints{};
	};
}
