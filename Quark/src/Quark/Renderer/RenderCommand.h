#pragma once

#include "Quark/Core/Core.h"
#include "GraphicsAPI.h"

#include <thread>
#define QK_ASSERT_RENDER_THREAD() QK_CORE_ASSERT(std::this_thread::get_id() == ::Quark::RenderCommand::GetThreadId(), "Function must be called from the render thread!")

namespace Quark {

	class RenderCommand
	{
	public:
		static void Init()
		{
			s_ThreadId = std::this_thread::get_id();
			s_GraphicsAPI->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->SetClearColor(color);
		}

		static void Clear()
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->Clear();
		}

		static void SetCullFace(RenderCullFace face)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->SetCullFace(face);
		}

		static void SetDepthFunction(RenderDepthFunction func)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->SetDepthFunction(func);
		}

		static void Draw(uint32_t offset, uint32_t count)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->Draw(offset, count);
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->DrawIndexed(vertexArray, indexCount);
		}

		static void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->DrawIndexedInstanced(vertexArray, repeatCount, indexCount);
		}

		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->DrawLines(vertexArray, vertexCount);
		}

		static void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->DrawIndexedLines(vertexArray, indexCount);
		}

		static void SetLineThickness(float thickness)
		{
			QK_ASSERT_RENDER_THREAD();
			s_GraphicsAPI->SetLineThickness(thickness);
		}

		static float GetLineThickness()
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->GetLineThickness();
		}

		static Scope<GraphicsContext> CreateGraphicsContext(void* windowHandle)
		{
			return s_GraphicsAPI->CreateGraphicsContext(windowHandle);
		}

		static Ref<VertexBuffer> CreateVertexBuffer(const void* vertices, size_t size)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateVertexBuffer(vertices, size);
		}

		static Ref<VertexBuffer> CreateVertexBuffer(size_t size)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateVertexBuffer(size);
		}

		static Ref<IndexBuffer> CreateIndexBuffer(const uint32_t* indices, uint32_t count)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateIndexBuffer(indices, count);
		}

		static Ref<IndexBuffer> CreateIndexBuffer(uint32_t count)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateIndexBuffer(count);
		}

		static Ref<Cubemap> CreateCubemap(const CubemapSpecification& spec)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateCubemap(spec);
		}

		static Ref<Font> CreateFont(std::string_view filepath, uint32_t fontSize)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateFont(filepath, fontSize);
		}

		static Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateFramebuffer(spec);
		}

		static Ref<Shader> CreateShader(std::string_view filepath)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateShader(filepath);
		}

		static Ref<Shader> CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateShader(name, vertexSource, fragmentSource);
		}

		static Ref<Shader> CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateShader(name, vertexSource, geometrySource, fragmentSource);
		}

		static Ref<Texture2D> CreateTexture2D(const Texture2DSpecification& spec)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateTexture2D(spec);
		}

		static Ref<Texture2D> CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor = {})
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateTexture2D(filepath, descriptor);
		}

		static Ref<Texture2DArray> CreateTexture2DArray(const TextureArraySpecification& spec)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateTexture2DArray(spec);
		}

		static Ref<UniformBuffer> CreateUniformBuffer(size_t size, uint32_t binding)
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateUniformBuffer(size, binding);
		}

		static Ref<VertexArray> CreateVertexArray()
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->CreateVertexArray();
		}

		static const char* GetAPIName()
		{
			return s_GraphicsAPI->GetName();
		}

		static std::string GetSpecification()
		{
			QK_ASSERT_RENDER_THREAD();
			return s_GraphicsAPI->GetSpecification();
		}

		static const HardwareConstraints& GetHardwareConstraints()
		{
			return s_GraphicsAPI->GetHardwareConstraints();
		}

		static std::thread::id GetThreadId() { return s_ThreadId; }

	private:
		static std::thread::id s_ThreadId;
		static Scope<GraphicsAPI> s_GraphicsAPI;
	};
}
