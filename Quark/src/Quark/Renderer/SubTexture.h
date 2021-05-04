#pragma once

#include "../Core/Core.h"

#include "Texture.h"

namespace Quark {

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& coord, const glm::vec2& subTextureSize);

		const Ref<Texture2D>& GetTexture() const { return m_Texture; }
		const glm::vec2* GetCoords() const { return m_TextureCoords; }

	private:
		Ref<Texture2D> m_Texture;
		glm::vec2 m_TextureCoords[4];
	};
}
