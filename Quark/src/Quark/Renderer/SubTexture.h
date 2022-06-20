#pragma once

#include "Quark/Core/Core.h"
#include "Texture.h"

namespace Quark {

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const Vec2f& coord, const Vec2f& subTextureSize);

		const Ref<Texture2D>& GetTexture() const { return m_Texture; }
		const Vec2f* GetCoords() const { return m_TextureCoords; }

	private:
		Ref<Texture2D> m_Texture;
		Vec2f m_TextureCoords[4];
	};
}
