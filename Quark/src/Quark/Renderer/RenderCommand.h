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

		static void SetClearColor(const Vec4f& color)
		{
			GraphicsAPI::Instance->SetClearColor(color);
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
