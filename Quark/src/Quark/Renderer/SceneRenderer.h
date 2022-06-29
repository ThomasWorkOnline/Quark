#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Scene/Scene.h"

namespace Quark {

	class SceneRenderer
	{
	public:
		SceneRenderer() = delete;
		SceneRenderer& operator=(const SceneRenderer&) = delete;

		static void SetActiveScene(const Ref<Scene>& scene);
		static void SetEnvironment(std::string_view filepath);

		static void OnRender();
		static void OnViewportResized(uint32_t width, uint32_t height);

	private:
		static void Dispose();
		static void OnNewActiveScene();

		static void InitEnvironment(std::string_view environmentFilepath);

		static Vec2i s_ViewportSize;
		static Ref<Scene> s_ActiveScene;

		friend class Application;
	};
}
