#include "SubTexture.h"

namespace Quark {

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& coord, const glm::vec2& subTextureSize)
		: m_Texture(texture)
	{
		glm::vec2 start = glm::vec2(coord.x * subTextureSize.x / texture->GetWidth(), coord.y * subTextureSize.y / texture->GetHeight());
		glm::vec2 end = glm::vec2(subTextureSize.x / texture->GetWidth(), subTextureSize.y / texture->GetHeight());

		m_TextureCoords[0] = { start };
		m_TextureCoords[1] = { start.x + end.x, start.y };
		m_TextureCoords[2] = { start.x + end.x, start.y + end.y };
		m_TextureCoords[3] = { start.x, start.y + end.y };
	}
}
