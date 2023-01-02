#include "qkpch.h"
#include "OpenGLGraphicsAPI.h"

#include "OpenGLCommandBuffer.h"
#include "OpenGLBuffer.h"
#include "OpenGLCubemap.h"
#include "OpenGLFont.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLPipeline.h"
#include "OpenGLRenderPass.h"
#include "OpenGLSampler.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>
#include <sstream>

namespace Quark {

	void OpenGLGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();

		// Framebuffer capabilities
		{
			GLint maxWidth, maxHeight, maxLayers, maxAttachments;
			glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &maxWidth);
			m_Capabilities.Framebuffer.MaxWidth = maxWidth;

			glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &maxHeight);
			m_Capabilities.Framebuffer.MaxHeight = maxHeight;

			glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &maxLayers);
			m_Capabilities.Framebuffer.MaxLayers = maxLayers;

			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);
			m_Capabilities.Framebuffer.MaxAttachments = maxAttachments;
		}

		// Sampler capabilities
		{
			GLint maxSamplers;
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxSamplers);
			m_Capabilities.Sampler.MaxTextureUnits = maxSamplers;

			GLfloat maxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
			m_Capabilities.Sampler.MaxAnisotropy = maxAnisotropy;
		}

		// Uniform buffer capabilities
		{
			GLint maxBlockSize, maxBindings;
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxBlockSize);
			m_Capabilities.UniformBuffer.MaxBufferSize = maxBlockSize;

			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings);
			m_Capabilities.UniformBuffer.MaxPerStageBuffers = maxBindings;
		}

		// Texture capabilities
		{
			GLint maxTexture2dSize, maxTexture3dSize, maxTextureArrayLayers, maxCubemapSize;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexture2dSize);
			m_Capabilities.Texture.Max2DSize = maxTexture2dSize;

			glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &maxTexture3dSize);
			m_Capabilities.Texture.Max3DSize = maxTexture3dSize;

			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxTextureArrayLayers);
			m_Capabilities.Texture.MaxArrayLayers = maxTextureArrayLayers;

			glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubemapSize);
			m_Capabilities.Texture.MaxCubemapSize = maxCubemapSize;
		}
	}

	RHIVersion OpenGLGraphicsAPI::GetRHIVersion() const
	{
		return { (uint32_t)GLVersion.major, (uint32_t)GLVersion.minor, 0 };
	}

	Scope<CommandBuffer> OpenGLGraphicsAPI::CreateCommandBuffer()
	{
		return CreateScope<OpenGLCommandBuffer>();
	}

	Scope<Pipeline> OpenGLGraphicsAPI::CreatePipeline(const PipelineSpecification& spec)
	{
		return CreateScope<OpenGLPipeline>(spec);
	}

	Scope<RenderPass> OpenGLGraphicsAPI::CreateRenderPass(const RenderPassSpecification& spec)
	{
		return CreateScope<OpenGLRenderPass>(spec);
	}

	Scope<VertexBuffer> OpenGLGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return CreateScope<OpenGLVertexBuffer>(vertices, size);
	}

	Scope<VertexBuffer> OpenGLGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return CreateScope<OpenGLVertexBuffer>(size);
	}

	Scope<IndexBuffer> OpenGLGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count, IndexType indexType)
	{
		return CreateScope<OpenGLIndexBuffer>(indices, count, indexType);
	}

	Scope<IndexBuffer> OpenGLGraphicsAPI::CreateIndexBuffer(uint32_t count, IndexType indexType)
	{
		return CreateScope<OpenGLIndexBuffer>(count, indexType);
	}

	Scope<Cubemap> OpenGLGraphicsAPI::CreateCubemap(const CubemapSpecification& spec)
	{
		return CreateScope<OpenGLCubemap>(spec);
	}

	Scope<Font> OpenGLGraphicsAPI::CreateFont(std::string_view filepath, uint32_t fontSize)
	{
		return CreateScope<OpenGLFont>(filepath, fontSize);
	}

	Scope<Framebuffer> OpenGLGraphicsAPI::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return CreateScope<OpenGLFramebuffer>(spec);
	}

	Scope<FramebufferAttachment> OpenGLGraphicsAPI::CreateFramebufferAttachment(const FramebufferAttachmentSpecification& spec)
	{
		return CreateScope<OpenGLFramebufferAttachment>(spec);
	}

	Scope<Shader> OpenGLGraphicsAPI::CreateShader(std::string_view filepath)
	{
		return CreateScope<OpenGLShader>(filepath);
	}

	Scope<Shader> OpenGLGraphicsAPI::CreateShader(
		std::string_view name,
		std::string_view vertexSource,
		std::string_view fragmentSource)
	{
		return CreateScope<OpenGLShader>(name, vertexSource, fragmentSource);
	}

	Scope<Shader> OpenGLGraphicsAPI::CreateShader(
		std::string_view name,
		std::string_view vertexSource,
		std::string_view geometrySource,
		std::string_view fragmentSource)
	{
		return CreateScope<OpenGLShader>(name, vertexSource, geometrySource, fragmentSource);
	}

	Scope<Shader> OpenGLGraphicsAPI::CreateShader(
		std::string_view name,
		std::span<const uint32_t> vertexSpirv,
		std::span<const uint32_t> fragmentSpirv)
	{
		return CreateScope<OpenGLShader>(name, vertexSpirv, fragmentSpirv);
	}

	Scope<Shader> OpenGLGraphicsAPI::CreateShader(std::string_view name,
		std::span<const uint32_t> vertexSpirv,
		std::span<const uint32_t> geometrySpirv,
		std::span<const uint32_t> fragmentSpirv)
	{
		return CreateScope<OpenGLShader>(name, vertexSpirv, geometrySpirv, fragmentSpirv);
	}

	Scope<Sampler> OpenGLGraphicsAPI::CreateSampler(const SamplerSpecification& spec)
	{
		return CreateScope<OpenGLSampler>(spec);
	}

	Scope<Texture2D> OpenGLGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return CreateScope<OpenGLTexture2D>(spec);
	}

	Scope<Texture2D> OpenGLGraphicsAPI::CreateTexture2D(std::string_view filepath)
	{
		return CreateScope<OpenGLTexture2D>(filepath);
	}

	Scope<Texture2DArray> OpenGLGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return CreateScope<OpenGLTexture2DArray>(spec);
	}

	Scope<UniformBuffer> OpenGLGraphicsAPI::CreateUniformBuffer(size_t size)
	{
		return CreateScope<OpenGLUniformBuffer>(size);
	}

	const char* OpenGLGraphicsAPI::GetDeviceName() const
	{
		static_assert(sizeof(GLubyte) == sizeof(char));
		return (const char*)glGetString(GL_RENDERER);
	}

	std::string OpenGLGraphicsAPI::GetSpecification() const
	{
		std::stringstream ss;
		ss << "OpenGL Info:\n";
		ss << "|  " << glGetString(GL_VENDOR) << '\n';
		ss << "|  " << glGetString(GL_RENDERER) << '\n';
		ss << "|  " << glGetString(GL_VERSION) << '\n';
		ss << "|  Per-stage Capabilities:\n";
		ss << "|    Max uniform buffers = " << m_Capabilities.UniformBuffer.MaxPerStageBuffers << '\n';
		ss << "|    Max samplers        = " << m_Capabilities.Sampler.MaxTextureUnits;
		return ss.str();
	}
}
