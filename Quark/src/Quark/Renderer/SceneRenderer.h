#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Scene/Scene.h"

#include "Buffer.h"
#include "Cubemap.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Shader.h"
#include "UniformBuffer.h"

namespace Quark {

	class SceneRenderer
	{
	public:
		static void SetContext(const Ref<Scene>& scene);
		static void SetEnvironment(std::string_view filepath);

	private:
		static void OnRender();
		static void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight);

		static void GeometryPass();

		static void Initialize();
		static void Shutdown();

		static void NewEnvironment(std::string_view filepath);

	private:
		static Ref<Scene> s_Scene;
		static Vec2i s_ViewportSize;

		friend class Application;
	};
}
