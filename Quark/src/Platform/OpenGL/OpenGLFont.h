#pragma once

#include "Quark/Renderer/Font.h"
#include <vector>

struct FT_FaceRec_;
typedef struct FT_FaceRec_* Face;

typedef unsigned int GLuint;

static constexpr uint8_t s_ASCII_Start = 32;
static constexpr uint8_t s_ASCII_End   = 255;
static constexpr uint8_t s_GlyphCount  = s_ASCII_End - s_ASCII_Start;

namespace Quark {

	class OpenGLFont : public Font
	{
	public:
		OpenGLFont(std::string_view filepath, uint32_t fontSize);
		virtual ~OpenGLFont() override;

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

		virtual const Glyph& GetGlyph(uint8_t charcode) const override;
		virtual uint32_t GetGlyphCount() const override { return static_cast<uint32_t>(s_GlyphCount); }

		virtual uint32_t GetFontSize() const override { return m_FontSize; }
		virtual uint32_t GetAtlasWidth() const override { return m_AtlasWidth; }
		virtual uint32_t GetAtlasHeight() const override { return m_AtlasHeight; }

		virtual std::string_view GetStyleName() const override;
		virtual std::string_view GetFamilyName() const override;

		virtual bool operator==(const Texture& other) const override;

		// Non-Copyable
		OpenGLFont(const OpenGLFont&) = delete;
		OpenGLFont& operator=(const OpenGLFont&) = delete;

	private:
		void Init();
		void Shutdown();

	private:
		GLuint m_RendererID = 0;
		uint32_t m_FontSize = 0;
		uint32_t m_AtlasWidth = 0, m_AtlasHeight = 0;

		Face m_Face;
		Glyph m_Glyphs[s_GlyphCount];
	};
}
