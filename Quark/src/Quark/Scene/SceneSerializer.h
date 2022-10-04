#pragma once

#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace Quark {

	class SceneSerializer
	{
	public:
		void SerializeRuntime(Scene* scene, std::string_view sceneName);
		void DeserializeRuntime(Scene* scene, std::string_view sceneName);
	};
}
