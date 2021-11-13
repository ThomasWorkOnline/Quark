#pragma once

#include "../Core/Core.h"

#include "RenderingAPI.h"

namespace Quark {

	class RenderCommand
	{
	public:
		static void CreateRenderingAPI(RenderingAPI::API api)
		{
			s_RenderingAPI = RenderingAPI::Create(api);
		}

		static void Init()
		{
			s_RenderingAPI->Init();
			QK_CORE_TRACE("Initialized the rendering API");
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RenderingAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			s_RenderingAPI->SetClearColor(color);
		}

		static void Clear()
		{
			s_RenderingAPI->Clear();
		}

		static void SetCullFace(RenderCullFace face)
		{
			s_RenderingAPI->SetCullFace(face);
		}

		static void SetDepthFunction(RenderDepthFunction func)
		{
			s_RenderingAPI->SetDepthFunction(func);
		}

		static void Draw(uint32_t offset, uint32_t count)
		{
			s_RenderingAPI->Draw(offset, count);
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RenderingAPI->DrawIndexed(vertexArray, indexCount);
		}
		
		static void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0)
		{
			s_RenderingAPI->DrawIndexedInstanced(vertexArray, repeatCount, indexCount);
		}

		static void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RenderingAPI->DrawIndexedLines(vertexArray, indexCount);
		}

		static int32_t GetTextureSlotsCount()
		{
			return s_RenderingAPI->GetMaxTextureSlotsCount();
		}

		static int32_t GetMaxTextureSize()
		{
			return s_RenderingAPI->GetMaxTextureSize();
		}

		static int32_t GetMaxTextureLayers()
		{
			return s_RenderingAPI->GetMaxTextureLayers();
		}

		static std::string GetSpecification()
		{
			return s_RenderingAPI->GetSpecification();
		}

	private:
		static Scope<RenderingAPI> s_RenderingAPI;
	};
}
