#include "qkpch.h"
#include "OpenGLFont.h"

#pragma warning(push, 0)
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#pragma warning(pop)

#include <glad/glad.h>

namespace Quark {

	static uint32_t s_FontCount = 0;
	static FT_Library s_Library;

	static constexpr uint8_t s_ASCII_Start  = 32;
	static constexpr uint8_t s_ASCII_End    = 255;
	static constexpr uint8_t s_GlyphCount   = s_ASCII_End - s_ASCII_Start;

	OpenGLFont::OpenGLFont(std::string_view filepath, uint32_t fontSize)
		: m_FontSize(fontSize)
	{
		QK_PROFILE_FUNCTION();

		if (s_FontCount == 0)
			Init();

		++s_FontCount;

		FT_Error error;
		error = FT_New_Face(s_Library, filepath.data(), 0, &m_Face);
		QK_CORE_ASSERT(error == FT_Err_Ok, "Could not load new font face at path: '{0}'", filepath);

		error = FT_Set_Pixel_Sizes(m_Face, 0, fontSize);
		QK_CORE_ASSERT(error == FT_Err_Ok, "Could not set font dimensions.");

		QK_CORE_TRACE("Loading {0} glyphs from font at path: '{1}'", s_GlyphCount, filepath);

		FT_GlyphSlot g = m_Face->glyph;
		uint32_t w = 0;
		uint32_t h = 0;

		for (uint8_t i = s_ASCII_Start; i < s_ASCII_End; i++)
		{
			if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER))
			{
				QK_CORE_TRACE("Failed to load char: '{0}'", i);
				continue;
			}

			w += g->bitmap.width;
			h = std::max(h, g->bitmap.rows);
		}

		m_AtlasWidth = w;
		m_AtlasHeight = h;
		m_Glyphs.reserve(s_GlyphCount);

		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
		glClearTexImage(m_RendererID, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

		uint32_t x = 0;
		for (uint32_t i = 0; i < s_GlyphCount; i++)
		{
			if (FT_Load_Char(m_Face, i + s_ASCII_Start, FT_LOAD_RENDER))
				continue;

			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

			m_Glyphs.push_back({
				{ g->bitmap.width, g->bitmap.rows },
				{ g->bitmap_left, g->bitmap_top },
				{ g->advance.x, g->advance.y },
				x
			});

			x += g->bitmap.width;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		FT_Done_Face(m_Face);
	}

	OpenGLFont::~OpenGLFont()
	{
		QK_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);

		--s_FontCount;

		if (s_FontCount == 0)
			Shutdown();
	}

	void OpenGLFont::Attach(uint32_t textureSlot) const
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void OpenGLFont::Detach() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	const Glyph& OpenGLFont::GetGlyph(uint8_t charcode) const
	{
		QK_CORE_ASSERT(charcode >= s_ASCII_Start, "Invalid charcode");
		return m_Glyphs.at(charcode - s_ASCII_Start);
	}

	void OpenGLFont::Init()
	{
		QK_PROFILE_FUNCTION();

		FT_Error error = FT_Init_FreeType(&s_Library);
		QK_CORE_ASSERT(error == FT_Err_Ok, "Could not initialize freetype!");
	}

	void OpenGLFont::Shutdown()
	{
		QK_PROFILE_FUNCTION();

		FT_Done_FreeType(s_Library);
	}
}
