#pragma once

#include "Quark/Core/Core.h"

#include "APICapabilities.h"

#include "Buffer.h"
#include "Cubemap.h"
#include "CommandBuffer.h"
#include "Font.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderModes.h"
#include "RenderPass.h"
#include "Sampler.h"
#include "Shader.h"
#include "Texture.h"
#include "UniformBuffer.h"

namespace Quark {

	class GraphicsAPI
	{
	public:
		virtual                             ~GraphicsAPI() = default;
		virtual void                         Init() = 0;
										     
		virtual RHIVersion                   GetRHIVersion() const = 0;
									         
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
		virtual Scope<FramebufferAttachment> CreateFramebufferAttachment(const FramebufferAttachmentSpecification& spec) = 0;
		
		virtual Scope<Shader>                CreateShader(std::string_view filepath) = 0;
		virtual Scope<Shader>                CreateShader(std::string_view name, SpirvView vertexSource, SpirvView fragmentSource) = 0;
		virtual Scope<Shader>                CreateShader(std::string_view name, SpirvView vertexSource, SpirvView geometrySource, SpirvView fragmentSource) = 0;

		virtual Scope<Shader>                CreateShaderLegacy(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource) = 0;
		virtual Scope<Shader>                CreateShaderLegacy(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource) = 0;
									       
		virtual Scope<Sampler>               CreateSampler(const SamplerSpecification& spec) = 0;

		virtual Scope<Texture2D>             CreateTexture2D(const Texture2DSpecification& spec) = 0;
		virtual Scope<Texture2D>             CreateTexture2D(std::string_view filepath) = 0;
									       
		virtual Scope<Texture2DArray>        CreateTexture2DArray(const Texture2DArraySpecification& spec) = 0;
									       
		virtual Scope<UniformBuffer>         CreateUniformBuffer(const UniformBufferSpecification& spec) = 0;
									       
		virtual const char*                  GetName() const = 0;
		virtual std::string                  GetSpecification() const = 0;

		const                                GraphicsAPICapabilities& GetCapabilities() const { return m_Capabilities; }

		// To be defined by each supported platform
		static RHI                           GetDefaultRHIForPlatform();
		static RHI                           GetAPI() { return s_API; }

	protected:
		GraphicsAPICapabilities m_Capabilities{};

	private:
		static inline Scope<GraphicsAPI> Instantiate(RHI api)
		{
			s_API = api;
			return CreateRHIForPlatform(api);
		}

		// To be defined by each supported platform
		static Scope<GraphicsAPI> CreateRHIForPlatform(RHI api);

	private:
		friend class Renderer;
		static inline RHI s_API = RHI::None;
	};
}
