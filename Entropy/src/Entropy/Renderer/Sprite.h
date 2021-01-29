#pragma once

#include "Texture.h"

namespace Entropy {

	class Sprite
	{
	public:
		Sprite() = default;

	private:
		Ref<Texture2D> m_Texture;
	};
}
