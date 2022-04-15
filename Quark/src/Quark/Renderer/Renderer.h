#pragma once

#include "Quark/Core/Core.h"

#include "Renderer2D.h"
#include "Renderer3D.h"

namespace Quark {

	class Renderer
	{
	public:
		Renderer() = delete;
		Renderer& operator=(const Renderer& other) = delete;

		static void Initialize();
		static void Dispose();

		static void OnWindowResized(uint32_t width, uint32_t height);

	private:
		static uint32_t s_ViewportWidth, s_ViewportHeight;
	};
}
