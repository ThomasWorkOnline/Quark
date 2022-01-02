#pragma once

#include "../Core/Core.h"

#include "RenderingAPI.h"

namespace Quark {

	class RenderCommand
	{
	public:
		static void Init()
		{
			RenderingApi->Init();
			QK_CORE_TRACE("Initialized the rendering API");
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			RenderingApi->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			RenderingApi->SetClearColor(color);
		}

		static void Clear()
		{
			RenderingApi->Clear();
		}

		static void SetCullFace(RenderCullFace face)
		{
			RenderingApi->SetCullFace(face);
		}

		static void SetDepthFunction(RenderDepthFunction func)
		{
			RenderingApi->SetDepthFunction(func);
		}

		static void Draw(uint32_t offset, uint32_t count)
		{
			RenderingApi->Draw(offset, count);
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			RenderingApi->DrawIndexed(vertexArray, indexCount);
		}
		
		static void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0)
		{
			RenderingApi->DrawIndexedInstanced(vertexArray, repeatCount, indexCount);
		}

		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			RenderingApi->DrawLines(vertexArray, vertexCount);
		}

		static void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			RenderingApi->DrawIndexedLines(vertexArray, indexCount);
		}

		static void SetLineThickness(float thickness)
		{
			RenderingApi->SetLineThickness(thickness);
		}

		static float GetLineThickness()
		{
			return RenderingApi->GetLineThickness();
		}

		static int32_t GetTextureSlotsCount()
		{
			return RenderingApi->GetMaxTextureSlotsCount();
		}

		static int32_t GetMaxTextureSize()
		{
			return RenderingApi->GetMaxTextureSize();
		}

		static int32_t GetMaxTextureLayers()
		{
			return RenderingApi->GetMaxTextureLayers();
		}

		static std::string GetSpecification()
		{
			return RenderingApi->GetSpecification();
		}
	};
}
