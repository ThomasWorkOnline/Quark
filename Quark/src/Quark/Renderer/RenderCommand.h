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

		static void Submit()
		{
			GraphicsAPI::Instance->WaitAndRender();
		}

		static void WaitForFences()
		{
			GraphicsAPI::Instance->WaitForFences();
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

		static void SetCullFace(RenderCullMode face)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->SetCullFace(face);
		}

		static void SetDepthFunction(RenderDepthFunction func)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->SetDepthFunction(func);
		}

		static void Draw(uint32_t offset, uint32_t vertexCount)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->Draw(offset, vertexCount);
		}

		static void DrawIndexed(uint32_t indexCount)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->DrawIndexed(indexCount);
		}

		static void DrawIndexedInstanced(uint32_t repeatCount, uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->DrawIndexedInstanced(repeatCount, indexCount);
		}

		static void DrawLines(uint32_t vertexCount)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->DrawLines(vertexCount);
		}

		static void DrawIndexedLines(uint32_t indexCount = 0)
		{
			QK_ASSERT_RENDER_THREAD();
			GraphicsAPI::Instance->DrawIndexedLines(indexCount);
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

		static std::thread::id GetThreadId() { return s_ThreadId; }

	private:
		static std::thread::id s_ThreadId;
	};
}
