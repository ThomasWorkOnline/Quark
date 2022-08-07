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

	static void OnOpenGLMessage(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         QK_CORE_ASSERT(false, message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       QK_CORE_WARN(message);          return;
			case GL_DEBUG_SEVERITY_LOW:          QK_CORE_INFO(message);          return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: QK_CORE_TRACE(message);         return;

			QK_ASSERT_DEFAULT("OnOpenGLMessage had an unknown severity level");
		}
	}

	void OpenGLGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();

#ifdef QK_DEBUG
		if (GLVersion.major >= 4 && GLVersion.minor >= 3)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(OnOpenGLMessage, nullptr); // <-- This is not supported on OpenGL 4.2 or lower
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		}
#endif

		// Gamma correction
		glEnable(GL_FRAMEBUFFER_SRGB);

		// Alpha and Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Face Culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		//           ^^^-- we use a counter-clockwise winding order

		// Depth Testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Filtering
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH); // <-- NOTE: this massively slows down line rendering

		// Extensions
		//glEnable(GL_TEXTURE_2D_ARRAY_EXT);

		// Experimental
		//glEnable(GL_PROGRAM_POINT_SIZE);

		{
			GLint maxWidth, maxHeight;
			glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &maxWidth);
			glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &maxHeight);
			m_Constraints.FramebufferConstraints.MaxWidth = maxWidth;
			m_Constraints.FramebufferConstraints.MaxHeight = maxHeight;

			GLint maxAttachments;
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);
			m_Constraints.FramebufferConstraints.MaxAttachments = maxAttachments;
		}

		{
			GLint maxBlockSize;
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxBlockSize);
			m_Constraints.UniformBufferConstraints.MaxBufferSize = maxBlockSize;

			GLint maxBindings;
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings);
			m_Constraints.UniformBufferConstraints.MaxBindings = maxBindings;
		}

		{
			GLint maxTextureSlots;
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureSlots);
			m_Constraints.TextureConstraints.MaxTextureSlots = maxTextureSlots;

			GLint maxTextureSize;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
			m_Constraints.TextureConstraints.MaxPixelSize = maxTextureSize;

			GLint maxArrayTextureLayers;
			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureLayers);
			m_Constraints.TextureConstraints.MaxTextureArrayLayers = maxArrayTextureLayers;
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

	void OpenGLGraphicsAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLGraphicsAPI::Draw(uint32_t offset, uint32_t count)
	{
		glDrawArrays(GL_TRIANGLES, offset, count);
	}

	void OpenGLGraphicsAPI::DrawIndexed(uint32_t indexCount)
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLGraphicsAPI::DrawIndexedInstanced(uint32_t repeatCount, uint32_t indexCount)
	{
		glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, repeatCount);
	}

	void OpenGLGraphicsAPI::DrawLines(uint32_t vertexCount)
	{
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLGraphicsAPI::DrawIndexedLines(uint32_t indexCount)
	{
		glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
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

	Ref<CommandBuffer> OpenGLGraphicsAPI::CreateCommandBuffer()
	{
		return CreateRef<OpenGLCommandBuffer>();
	}

	Ref<Pipeline> OpenGLGraphicsAPI::CreatePipeline(const PipelineSpecification& spec)
	{
		return CreateRef<OpenGLPipeline>(spec);
	}

	Ref<RenderPass> OpenGLGraphicsAPI::CreateRenderPass(const RenderPassSpecification& spec)
	{
		return CreateRef<OpenGLRenderPass>(spec);
	}

	Ref<VertexBuffer> OpenGLGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return CreateRef<OpenGLVertexBuffer>(vertices, size);
	}

	Ref<VertexBuffer> OpenGLGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return CreateRef<OpenGLVertexBuffer>(size);
	}

	Ref<IndexBuffer> OpenGLGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return CreateRef<OpenGLIndexBuffer>(indices, count);
	}

	Ref<IndexBuffer> OpenGLGraphicsAPI::CreateIndexBuffer(uint32_t count)
	{
		return CreateRef<OpenGLIndexBuffer>(count);
	}

	Ref<Cubemap> OpenGLGraphicsAPI::CreateCubemap(const CubemapSpecification& spec)
	{
		return CreateRef<OpenGLCubemap>(spec);
	}

	Ref<Font> OpenGLGraphicsAPI::CreateFont(std::string_view filepath, uint32_t fontSize)
	{
		return CreateRef<OpenGLFont>(filepath, fontSize);
	}

	Ref<Framebuffer> OpenGLGraphicsAPI::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return CreateRef<OpenGLFramebuffer>(spec);
	}

	Ref<FramebufferAttachment> OpenGLGraphicsAPI::CreateFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec)
	{
		return CreateRef<OpenGLFramebufferAttachment>(image, spec);
	}

	Ref<Shader> OpenGLGraphicsAPI::CreateShader(std::string_view filepath)
	{
		return CreateRef<OpenGLShader>(filepath);
	}

	Ref<Shader> OpenGLGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return CreateRef<OpenGLShader>(name, vertexSource, fragmentSource);
	}

	Ref<Shader> OpenGLGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return CreateRef<OpenGLShader>(name, vertexSource, geometrySource, fragmentSource);
	}

	Ref<Texture2D> OpenGLGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return CreateRef<OpenGLTexture2D>(spec);
	}

	Ref<Texture2D> OpenGLGraphicsAPI::CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor)
	{
		return CreateRef<OpenGLTexture2D>(filepath, descriptor);
	}

	Ref<Texture2DArray> OpenGLGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return CreateRef<OpenGLTexture2DArray>(spec);
	}

	Ref<UniformBuffer> OpenGLGraphicsAPI::CreateUniformBuffer(size_t size, uint32_t binding)
	{
		return CreateRef<OpenGLUniformBuffer>(size, binding);
	}

	std::string OpenGLGraphicsAPI::GetSpecification() const
	{
		std::stringstream ss;
		ss << "OpenGL Info:\n";
		ss << "|\t" << glGetString(GL_VENDOR) << '\n';
		ss << "|\t" << glGetString(GL_RENDERER) << '\n';
		ss << "|\t" << glGetString(GL_VERSION) << '\n';
		ss << "|\tUniform buffer bindings: " << m_Constraints.UniformBufferConstraints.MaxBindings << '\n';
		ss << "|\tHardware texture slots available: " << m_Constraints.TextureConstraints.MaxTextureSlots;
		return ss.str();
	}
}
