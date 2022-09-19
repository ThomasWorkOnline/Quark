#pragma once

#include "Quark/Renderer/Font.h"
#include "OpenGLTexture.h"

struct FT_FaceRec_;
typedef struct FT_FaceRec_* Face;

typedef unsigned int GLuint;

static constexpr uint8_t s_GlyphCount = 255;

namespace Quark {

	class OpenGLFont : public OpenGLTexture, public Font
	{
	public:
		OpenGLFont(std::string_view filepath, uint32_t fontSize);
		virtual ~OpenGLFont() final override;

		virtual const Glyph& GetGlyph(uint8_t charcode) const final override;
		virtual uint32_t GetGlyphCount() const final override;

		virtual uint32_t GetFontSize() const final override;
		virtual uint32_t GetAtlasWidth() const final override { return m_AtlasWidth; }
		virtual uint32_t GetAtlasHeight() const final override { return m_AtlasHeight; }

		virtual std::string_view GetStyleName() const final override;
		virtual std::string_view GetFamilyName() const final override;

		virtual const void* GetHandle() const final override { return static_cast<const OpenGLTexture*>(this); }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		OpenGLFont(const OpenGLFont&) = delete;
		OpenGLFont& operator=(const OpenGLFont&) = delete;

	private:
		void Init();
		void Shutdown();

	private:
		uint32_t m_AtlasWidth = 0, m_AtlasHeight = 0;

		Face m_Face;
		Glyph m_Glyphs[s_GlyphCount];
	};
}
