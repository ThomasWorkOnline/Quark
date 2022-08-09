#pragma once

#include "Quark/Core/Core.h"
#include "GraphicsAPI.h"

namespace Quark {

	class RenderCommand
	{
	public:
		static void Init()
		{
			GraphicsAPI::Instance->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			GraphicsAPI::Instance->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const Vec4f& color)
		{
			GraphicsAPI::Instance->SetClearColor(color);
		}

		static void Clear()
		{
			GraphicsAPI::Instance->Clear();
		}

		static void SetCullFace(RenderCullMode face)
		{
			GraphicsAPI::Instance->SetCullFace(face);
		}

		static void SetDepthFunction(RenderDepthFunction func)
		{
			GraphicsAPI::Instance->SetDepthFunction(func);
		}

		static void SetLineThickness(float thickness)
		{
			GraphicsAPI::Instance->SetLineThickness(thickness);
		}

		static float GetLineThickness()
		{
			return GraphicsAPI::Instance->GetLineThickness();
		}
	};
}
