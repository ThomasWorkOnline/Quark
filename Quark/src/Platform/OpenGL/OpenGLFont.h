#pragma once

#include "Quark/Renderer/Font.h"

#include <vector>

struct FT_FaceRec_;
typedef struct FT_FaceRec_* Face;

namespace Quark {

	class OpenGLFont : public Font
	{
	public:
		OpenGLFont(std::string_view filepath, uint32_t fontSize);
		~OpenGLFont() override;

		void Attach(uint32_t textureSlot = 0) const override;
		void Detach() const override;

		const Glyph& GetGlyph(uint8_t charcode) const override;
		uint32_t GetGlyphCount() const override { return m_Glyphs.size(); }

		uint32_t GetFontSize() const override { return m_FontSize; }
		uint32_t GetAtlasWidth() const override { return m_AtlasWidth; }
		uint32_t GetAtlasHeight() const override { return m_AtlasHeight; }

		uint32_t GetRendererID() const override { return m_RendererID; }

		bool operator==(const Font& other) const override
		{
			return m_RendererID == ((OpenGLFont&)other).m_RendererID;
		}

	private:
		void Init();
		void Shutdown();

	private:
		Face m_Face;
		std::vector<Glyph> m_Glyphs;
		uint32_t m_FontSize;
		uint32_t m_AtlasWidth, m_AtlasHeight;
		uint32_t m_RendererID = 0;
	};
}
