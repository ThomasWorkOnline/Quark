#pragma once

#include "Quark/Core/Core.h"
#include "Texture.h"
#include "Shader.h"

namespace Quark {

	struct Material
	{
		Ref<Shader>    Shader;
		Ref<Texture2D> Albedo;
	};
}
