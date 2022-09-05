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
#include "OpenGLTextureArray.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>
#include <sstream>

namespace Quark {

	void OpenGLGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();

		{
			GLint maxWidth, maxHeight;
			glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &maxWidth);
			glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &maxHeight);
			m_Capabilities.FramebufferCapabilities.MaxWidth = maxWidth;
			m_Capabilities.FramebufferCapabilities.MaxHeight = maxHeight;

			GLint maxAttachments;
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);
			m_Capabilities.FramebufferCapabilities.MaxAttachments = maxAttachments;
		}

		{
			GLint maxBlockSize;
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxBlockSize);
			m_Capabilities.UniformBufferCapabilities.MaxBufferSize = maxBlockSize;

			GLint maxBindings;
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings);
			m_Capabilities.UniformBufferCapabilities.MaxBindings = maxBindings;
		}

		{
			GLint maxTextureSlots;
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureSlots);
			m_Capabilities.TextureCapabilities.MaxTextureSlots = maxTextureSlots;

			GLint maxTextureSize;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
			m_Capabilities.TextureCapabilities.MaxPixelSize = maxTextureSize;

			GLint maxArrayTextureLayers;
			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureLayers);
			m_Capabilities.TextureCapabilities.MaxTextureArrayLayers = maxArrayTextureLayers;
		}
	}

	GraphicsAPI::Version OpenGLGraphicsAPI::GetVersion() const
	{
		return { GLVersion.major, GLVersion.minor };
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

	Scope<IndexBuffer> OpenGLGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return CreateScope<OpenGLIndexBuffer>(indices, count);
	}

	Scope<IndexBuffer> OpenGLGraphicsAPI::CreateIndexBuffer(uint32_t count)
	{
		return CreateScope<OpenGLIndexBuffer>(count);
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

	Scope<Shader> OpenGLGraphicsAPI::CreateShader(std::string_view name, const SpirvSource& vertexSource, const SpirvSource& fragmentSource)
	{
		return CreateScope<OpenGLShader>(name, vertexSource, fragmentSource);
	}

	Scope<Shader> OpenGLGraphicsAPI::CreateShader(std::string_view name, const SpirvSource& vertexSource, const SpirvSource& geometrySource, const SpirvSource& fragmentSource)
	{
		return CreateScope<OpenGLShader>(name, vertexSource, geometrySource, fragmentSource);
	}

	Scope<Sampler2D> OpenGLGraphicsAPI::CreateSampler2D(const Sampler2DSpecification& spec)
	{
		return CreateScope<OpenGLSampler2D>(spec);
	}

	Scope<Texture2D> OpenGLGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return CreateScope<OpenGLTexture2D>(spec);
	}

	Scope<Texture2D> OpenGLGraphicsAPI::CreateTexture2D(std::string_view filepath, const TextureRenderModes& renderModes)
	{
		return CreateScope<OpenGLTexture2D>(filepath, renderModes);
	}

	Scope<Texture2DArray> OpenGLGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return CreateScope<OpenGLTexture2DArray>(spec);
	}

	Scope<UniformBuffer> OpenGLGraphicsAPI::CreateUniformBuffer(const UniformBufferSpecification& spec)
	{
		return CreateScope<OpenGLUniformBuffer>(spec);
	}

	std::string OpenGLGraphicsAPI::GetSpecification() const
	{
		std::stringstream ss;
		ss << "OpenGL Info:\n";
		ss << "|\t" << glGetString(GL_VENDOR) << '\n';
		ss << "|\t" << glGetString(GL_RENDERER) << '\n';
		ss << "|\t" << glGetString(GL_VERSION) << '\n';
		ss << "|\tUniform buffer bindings: " << m_Capabilities.UniformBufferCapabilities.MaxBindings << '\n';
		ss << "|\tHardware texture slots available: " << m_Capabilities.TextureCapabilities.MaxTextureSlots;
		return ss.str();
	}
}
