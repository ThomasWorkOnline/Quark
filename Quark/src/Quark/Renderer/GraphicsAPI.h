#pragma once

#include "Quark/Core/Core.h"

#include "RenderModes.h"
#include "APICapabilities.h"

#include "Buffer.h"
#include "Cubemap.h"
#include "CommandBuffer.h"
#include "Font.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"
#include "TextureArray.h"
#include "UniformBuffer.h"

namespace Quark {

	// Supported APIs
	enum class RHI
	{
		None = 0,
		OpenGL,
		Vulkan,

#ifdef QK_PLATFORM_APPLE
		Metal
#endif
	};

	// To be defined by each supported platform
	extern RHI GetDefaultRHIForPlatform();

	class GraphicsAPI
	{
	public:
		static Scope<GraphicsAPI> Instance;

		struct Version
		{
			int Major;
			int Minor;
		};

		virtual                             ~GraphicsAPI() = default;
		virtual void                         Init() = 0;
										     
		virtual                              Version GetVersion() const = 0;
									         
		virtual void                         SetClearColor(const Vec4f& rgba) = 0;
		virtual void                         SetCullFace(RenderCullMode face) = 0;
		virtual void                         SetDepthFunction(RenderDepthFunction func) = 0;
									         
		virtual void                         SetLineThickness(float thickness) = 0;
		virtual float                        GetLineThickness() const = 0;
									         
		virtual Scope<CommandBuffer>         CreateCommandBuffer() = 0;
									           
		virtual Scope<Pipeline>              CreatePipeline(const PipelineSpecification& spec) = 0;
		virtual Scope<RenderPass>            CreateRenderPass(const RenderPassSpecification& spec) = 0;
									         
		virtual Scope<VertexBuffer>          CreateVertexBuffer(const void* vertices, size_t size) = 0;
		virtual Scope<VertexBuffer>          CreateVertexBuffer(size_t size) = 0;
									         
		virtual Scope<IndexBuffer>           CreateIndexBuffer(const uint32_t* indices, uint32_t count) = 0;
		virtual Scope<IndexBuffer>           CreateIndexBuffer(uint32_t count) = 0;
									       
		virtual Scope<Cubemap>               CreateCubemap(const CubemapSpecification& spec) = 0;
									       
		virtual Scope<Font>                  CreateFont(std::string_view filepath, uint32_t fontSize) = 0;
									       
		virtual Scope<Framebuffer>           CreateFramebuffer(const FramebufferSpecification& spec) = 0;
		virtual Scope<FramebufferAttachment> CreateFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec) = 0;
		
		virtual Scope<Shader>                CreateShader(std::string_view filepath) = 0;
		virtual Scope<Shader>                CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource) = 0;
		virtual Scope<Shader>                CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource) = 0;
									       
		virtual Scope<Texture2D>             CreateTexture2D(const Texture2DSpecification& spec) = 0;
		virtual Scope<Texture2D>             CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor = {}) = 0;
									       
		virtual Scope<Texture2DArray>        CreateTexture2DArray(const Texture2DArraySpecification& spec) = 0;
									       
		virtual Scope<UniformBuffer>         CreateUniformBuffer(size_t size, uint32_t binding) = 0;
									       
		virtual const char*                  GetName() const = 0;
		virtual std::string                  GetSpecification() const = 0;
									         
		const                                GraphicsAPICapabilities& GetCapabilities() const { return m_Capabilities; }
										     
		static RHI                           GetDefaultForPlatform() { return GetDefaultRHIForPlatform(); }
		static RHI                           GetAPI() { return s_API; }
		static Scope<GraphicsAPI>            Instantiate(RHI api);

	protected:
		GraphicsAPICapabilities m_Capabilities{};

	private:
		static RHI s_API;
	};
}
