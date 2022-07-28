#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Scene/Components.h"

#include "Camera.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"

namespace Quark {

	class Renderer : public StaticClass
	{
	public:
		static void BeginFrame();
		static void EndFrame();

		static uint32_t GetFramesInFlight();
		static uint32_t GetCurrentFrameIndex();
		static ShaderLibrary& GetShaderLibrary();

	private:
		static void Initialize();
		static void Dispose();

		friend class Application;
	};
}
