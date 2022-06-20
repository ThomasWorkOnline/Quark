#include "qkpch.h"
#include "SubTexture.h"

namespace Quark {

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const Vec2f& coord, const Vec2f& subTextureSize)
		: m_Texture(texture)
	{
		auto start = Vec2f(coord.x * subTextureSize.x / texture->GetWidth(), coord.y * subTextureSize.y / texture->GetHeight());
		auto end = Vec2f(subTextureSize.x / texture->GetWidth(), subTextureSize.y / texture->GetHeight());

		m_TextureCoords[0] = { start };
		m_TextureCoords[1] = { start.x + end.x, start.y };
		m_TextureCoords[2] = { start.x + end.x, start.y + end.y };
		m_TextureCoords[3] = { start.x, start.y + end.y };
	}
}
