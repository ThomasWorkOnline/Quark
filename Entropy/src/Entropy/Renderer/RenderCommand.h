#pragma once

#include "../Core/Core.h"

#include "RenderingAPI.h"

namespace Entropy {

	class RenderCommand
	{
	public:
		static void Init()
		{
			s_RenderingAPI->Init();
			NT_TRACE("Initialized the rendering API");
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

		static void Draw(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RenderingAPI->Draw(vertexArray, indexCount);
		}

		static void DrawInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0)
		{
			s_RenderingAPI->DrawInstanced(vertexArray, repeatCount, indexCount);
		}

		static int32_t GetTextureSlotsCount()
		{
			return s_RenderingAPI->GetTextureSlotsCount();
		}

		static std::string GetSpecification()
		{
			return s_RenderingAPI->GetSpecification();
		}

	private:
		static Scope<RenderingAPI> s_RenderingAPI;
	};
}
