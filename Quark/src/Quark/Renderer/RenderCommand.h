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
			s_RenderingApi->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->SetClearColor(color);
		}

		static void Clear()
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->Clear();
		}

		static void SetCullFace(RenderCullFace face)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->SetCullFace(face);
		}

		static void SetDepthFunction(RenderDepthFunction func)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->SetDepthFunction(func);
		}

		static void Draw(uint32_t offset, uint32_t count)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->Draw(offset, count);
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->DrawIndexed(vertexArray, indexCount);
		}

		static void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->DrawIndexedInstanced(vertexArray, repeatCount, indexCount);
		}

		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->DrawLines(vertexArray, vertexCount);
		}

		static void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->DrawIndexedLines(vertexArray, indexCount);
		}

		static void SetLineThickness(float thickness)
		{
			QK_ASSERT_RENDER_THREAD();
			s_RenderingApi->SetLineThickness(thickness);
		}

		static float GetLineThickness()
		{
			QK_ASSERT_RENDER_THREAD();
			return s_RenderingApi->GetLineThickness();
		}

		static const char* GetAPIName()
		{
			QK_ASSERT_RENDER_THREAD();
			return s_RenderingApi->GetName();
		}

		static std::string GetSpecification()
		{
			QK_ASSERT_RENDER_THREAD();
			return s_RenderingApi->GetSpecification();
		}

		static const HardwareConstraints& GetHardwareConstraints()
		{
			return s_RenderingApi->GetHardwareConstraints();
		}

		static std::thread::id GetThreadId() { return s_ThreadId; }

	private:
		static std::thread::id s_ThreadId;
		static Scope<GraphicsAPI> s_RenderingApi;
	};
}
