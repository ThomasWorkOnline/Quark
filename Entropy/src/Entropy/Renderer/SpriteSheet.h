#pragma once

#include "../Core/Core.h"

#include "Texture.h"

namespace Entropy {

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& textureCoords, const glm::vec2& subTextureSize);

	private:
		Ref<Texture2D> m_Texture;
		glm::vec2 m_TextureCoords;
	};
}
