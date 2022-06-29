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
			GraphicsAPI::Instance->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const Vec4f& color)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->SetClearColor(color);
		}

		static void Clear()
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->Clear();
		}

		static void SetCullFace(RenderCullFace face)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->SetCullFace(face);
		}

		static void SetDepthFunction(RenderDepthFunction func)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->SetDepthFunction(func);
		}

		static void Draw(uint32_t offset, uint32_t count)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->Draw(offset, count);
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->DrawIndexed(vertexArray, indexCount);
		}

		static void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->DrawIndexedInstanced(vertexArray, repeatCount, indexCount);
		}

		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->DrawLines(vertexArray, vertexCount);
		}

		static void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->DrawIndexedLines(vertexArray, indexCount);
		}

		static void SetLineThickness(float thickness)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->SetLineThickness(thickness);
		}

		static float GetLineThickness()
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->GetLineThickness();
		}

		static Ref<VertexBuffer> CreateVertexBuffer(const void* vertices, size_t size)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateVertexBuffer(vertices, size);
		}

		static Ref<VertexBuffer> CreateVertexBuffer(size_t size)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateVertexBuffer(size);
		}

		static Ref<IndexBuffer> CreateIndexBuffer(const uint32_t* indices, uint32_t count)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateIndexBuffer(indices, count);
		}

		static Ref<IndexBuffer> CreateIndexBuffer(uint32_t count)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateIndexBuffer(count);
		}

		static Ref<Cubemap> CreateCubemap(const CubemapSpecification& spec)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateCubemap(spec);
		}

		static Ref<Font> CreateFont(std::string_view filepath, uint32_t fontSize)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateFont(filepath, fontSize);
		}

		static Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateFramebuffer(spec);
		}

		static Ref<Shader> CreateShader(std::string_view filepath)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateShader(filepath);
		}

		static Ref<Shader> CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateShader(name, vertexSource, fragmentSource);
		}

		static Ref<Shader> CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateShader(name, vertexSource, geometrySource, fragmentSource);
		}

		static Ref<Texture2D> CreateTexture2D(const Texture2DSpecification& spec)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateTexture2D(spec);
		}

		static Ref<Texture2D> CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor = {})
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateTexture2D(filepath, descriptor);
		}

		static Ref<Texture2DArray> CreateTexture2DArray(const Texture2DArraySpecification& spec)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateTexture2DArray(spec);
		}

		static Ref<UniformBuffer> CreateUniformBuffer(size_t size, uint32_t binding)
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateUniformBuffer(size, binding);
		}

		static Ref<VertexArray> CreateVertexArray()
		{
			QK_ASSERT_RENDER_THREAD();
			return GraphicsAPI::Instance->CreateVertexArray();
		}

		static const HardwareConstraints& GetHardwareConstraints()
		{
			return GraphicsAPI::Instance->GetHardwareConstraints();
		}

		static std::thread::id GetThreadId() { return s_ThreadId; }

	private:
		static std::thread::id s_ThreadId;
	};
}
