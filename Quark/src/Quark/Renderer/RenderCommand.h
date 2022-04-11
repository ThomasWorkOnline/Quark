#pragma once

#include "Quark/Core/Core.h"

#include "RenderingAPI.h"

namespace Quark {

	class RenderCommand
	{
	public:
		static void Init()
		{
			s_RenderingApi->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RenderingApi->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			s_RenderingApi->SetClearColor(color);
		}

		static void Clear()
		{
			s_RenderingApi->Clear();
		}

		static void SetCullFace(RenderCullFace face)
		{
			s_RenderingApi->SetCullFace(face);
		}

		static void SetDepthFunction(RenderDepthFunction func)
		{
			s_RenderingApi->SetDepthFunction(func);
		}

		static void Draw(uint32_t offset, uint32_t count)
		{
			s_RenderingApi->Draw(offset, count);
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RenderingApi->DrawIndexed(vertexArray, indexCount);
		}

		static void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0)
		{
			s_RenderingApi->DrawIndexedInstanced(vertexArray, repeatCount, indexCount);
		}

		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RenderingApi->DrawLines(vertexArray, vertexCount);
		}

		static void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RenderingApi->DrawIndexedLines(vertexArray, indexCount);
		}

		static void SetLineThickness(float thickness)
		{
			s_RenderingApi->SetLineThickness(thickness);
		}

		static float GetLineThickness()
		{
			return s_RenderingApi->GetLineThickness();
		}

		static int32_t GetMaxTextureSlots()
		{
			return s_RenderingApi->GetMaxTextureSlots();
		}

		static int32_t GetMaxTextureSize()
		{
			return s_RenderingApi->GetMaxTextureSize();
		}

		static int32_t GetMaxTextureLayers()
		{
			return s_RenderingApi->GetMaxTextureLayers();
		}

		static int32_t GetMaxUniformBufferSize()
		{
			return s_RenderingApi->GetMaxUniformBufferSize();
		}

		static const char* GetAPIName()
		{
			return s_RenderingApi->GetName();
		}

		static std::string GetSpecification()
		{
			return s_RenderingApi->GetSpecification();
		}

	private:
		static Scope<RenderingAPI> s_RenderingApi;
	};
}
