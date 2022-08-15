#pragma once

#include "Quark/Renderer/Font.h"
#include <vector>

struct FT_FaceRec_;
typedef struct FT_FaceRec_* Face;

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLFont : public Font
	{
	public:
		OpenGLFont(std::string_view filepath, uint32_t fontSize);
		virtual ~OpenGLFont() override;

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

		virtual const Glyph& GetGlyph(uint8_t charcode) const override;
		virtual uint32_t GetGlyphCount() const override { return static_cast<uint32_t>(m_Glyphs.size()); }

		virtual uint32_t GetFontSize() const override { return m_FontSize; }
		virtual uint32_t GetAtlasWidth() const override { return m_AtlasWidth; }
		virtual uint32_t GetAtlasHeight() const override { return m_AtlasHeight; }

		virtual bool operator==(const Font& other) const override
		{
			return m_RendererID == reinterpret_cast<decltype(*this)&>(other).m_RendererID;
		}

	private:
		void Init();
		void Shutdown();

	private:
		Face m_Face;
		std::vector<Glyph> m_Glyphs;

		uint32_t m_FontSize;
		uint32_t m_AtlasWidth, m_AtlasHeight;
		GLuint m_RendererID = 0;
	};
}
