#include "SpriteSheet.h"

namespace Entropy {

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& textureCoords, const glm::vec2& subTextureSize)
		: m_Texture(texture), m_TextureCoords(textureCoords)
	{
		
	}
}
