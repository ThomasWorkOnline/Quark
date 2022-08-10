#include "qkpch.h"
#include "OpenGLGraphicsAPI.h"

#include "OpenGLCommandBuffer.h"
#include "OpenGLBuffer.h"
#include "OpenGLCubemap.h"
#include "OpenGLFont.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLPipeline.h"
#include "OpenGLRenderPass.h"
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
			m_Capabilities.FramebufferConstraints.MaxWidth = maxWidth;
			m_Capabilities.FramebufferConstraints.MaxHeight = maxHeight;

			GLint maxAttachments;
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);
			m_Capabilities.FramebufferConstraints.MaxAttachments = maxAttachments;
		}

		{
			GLint maxBlockSize;
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxBlockSize);
			m_Capabilities.UniformBufferConstraints.MaxBufferSize = maxBlockSize;

			GLint maxBindings;
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings);
			m_Capabilities.UniformBufferConstraints.MaxBindings = maxBindings;
		}

		{
			GLint maxTextureSlots;
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureSlots);
			m_Capabilities.TextureConstraints.MaxTextureSlots = maxTextureSlots;

			GLint maxTextureSize;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
			m_Capabilities.TextureConstraints.MaxPixelSize = maxTextureSize;

			GLint maxArrayTextureLayers;
			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureLayers);
			m_Capabilities.TextureConstraints.MaxTextureArrayLayers = maxArrayTextureLayers;
		}
	}

	void OpenGLGraphicsAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLGraphicsAPI::SetCullFace(RenderCullMode face)
	{
		switch (face)
		{
		case RenderCullMode::None:
			glDisable(GL_CULL_FACE);
			break;
		case RenderCullMode::Front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case RenderCullMode::Back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case RenderCullMode::FrontAndBack:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT_AND_BACK);
			break;
		}
	}

	void OpenGLGraphicsAPI::SetDepthFunction(RenderDepthFunction func)
	{
		switch (func)
		{
		case RenderDepthFunction::Never:
			glDepthFunc(GL_NEVER);
			break;
		case RenderDepthFunction::Always:
			glDepthFunc(GL_ALWAYS);
			break;
		case RenderDepthFunction::NotEqual:
			glDepthFunc(GL_NOTEQUAL);
			break;
		case RenderDepthFunction::Less:
			glDepthFunc(GL_LESS);
			break;
		case RenderDepthFunction::LessEqual:
			glDepthFunc(GL_LEQUAL);
			break;
		case RenderDepthFunction::Greater:
			glDepthFunc(GL_GREATER);
			break;
		case RenderDepthFunction::GreaterEqual:
			glDepthFunc(GL_GEQUAL);
			break;
		}
	}

	GraphicsAPI::Version OpenGLGraphicsAPI::GetVersion() const
	{
		return { GLVersion.major, GLVersion.minor };
	}

	void OpenGLGraphicsAPI::SetClearColor(const Vec4f& rgba)
	{
		glClearColor(rgba.x, rgba.y, rgba.z, rgba.w);
	}

	void OpenGLGraphicsAPI::SetLineThickness(float thickness)
	{
		glLineWidth(thickness);
	}

	float OpenGLGraphicsAPI::GetLineThickness() const
	{
		GLfloat thickness;
		glGetFloatv(GL_LINE_WIDTH, &thickness);
		return thickness;
	}

	CommandBuffer* OpenGLGraphicsAPI::CreateCommandBuffer()
	{
		return new OpenGLCommandBuffer();
	}

	Pipeline* OpenGLGraphicsAPI::CreatePipeline(const PipelineSpecification& spec)
	{
		return new OpenGLPipeline(spec);
	}

	RenderPass* OpenGLGraphicsAPI::CreateRenderPass(const RenderPassSpecification& spec)
	{
		return new OpenGLRenderPass(spec);
	}

	VertexBuffer* OpenGLGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return new OpenGLVertexBuffer(vertices, size);
	}

	VertexBuffer* OpenGLGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return new OpenGLVertexBuffer(size);
	}

	IndexBuffer* OpenGLGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return new OpenGLIndexBuffer(indices, count);
	}

	IndexBuffer* OpenGLGraphicsAPI::CreateIndexBuffer(uint32_t count)
	{
		return new OpenGLIndexBuffer(count);
	}

	Cubemap* OpenGLGraphicsAPI::CreateCubemap(const CubemapSpecification& spec)
	{
		return new OpenGLCubemap(spec);
	}

	Font* OpenGLGraphicsAPI::CreateFont(std::string_view filepath, uint32_t fontSize)
	{
		return new OpenGLFont(filepath, fontSize);
	}

	Framebuffer* OpenGLGraphicsAPI::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return new OpenGLFramebuffer(spec);
	}

	FramebufferAttachment* OpenGLGraphicsAPI::CreateFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec)
	{
		return new OpenGLFramebufferAttachment(image, spec);
	}

	Shader* OpenGLGraphicsAPI::CreateShader(std::string_view filepath)
	{
		return new OpenGLShader(filepath);
	}

	Shader* OpenGLGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return new OpenGLShader(name, vertexSource, fragmentSource);
	}

	Shader* OpenGLGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return new OpenGLShader(name, vertexSource, geometrySource, fragmentSource);
	}

	Texture2D* OpenGLGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return new OpenGLTexture2D(spec);
	}

	Texture2D* OpenGLGraphicsAPI::CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor)
	{
		return new OpenGLTexture2D(filepath, descriptor);
	}

	Texture2DArray* OpenGLGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return new OpenGLTexture2DArray(spec);
	}

	UniformBuffer* OpenGLGraphicsAPI::CreateUniformBuffer(size_t size, uint32_t binding)
	{
		return new OpenGLUniformBuffer(size, binding);
	}

	std::string OpenGLGraphicsAPI::GetSpecification() const
	{
		std::stringstream ss;
		ss << "OpenGL Info:\n";
		ss << "|\t" << glGetString(GL_VENDOR) << '\n';
		ss << "|\t" << glGetString(GL_RENDERER) << '\n';
		ss << "|\t" << glGetString(GL_VERSION) << '\n';
		ss << "|\tUniform buffer bindings: " << m_Capabilities.UniformBufferConstraints.MaxBindings << '\n';
		ss << "|\tHardware texture slots available: " << m_Capabilities.TextureConstraints.MaxTextureSlots;
		return ss.str();
	}
}
