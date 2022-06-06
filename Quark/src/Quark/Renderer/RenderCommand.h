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

		static const char* GetAPIName()
		{
			QK_ASSERT_RENDER_THREAD();
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
