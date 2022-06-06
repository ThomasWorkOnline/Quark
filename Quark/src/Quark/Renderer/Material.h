#pragma once

#include "Quark/Core/Core.h"
#include "Texture.h"
#include "Shader.h"

namespace Quark {

	class Material
	{
	private:
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_Albedo;
	};
}
