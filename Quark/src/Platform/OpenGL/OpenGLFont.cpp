#include "OpenGLFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <GL/glew.h>

namespace Quark {

	static uint32_t s_FontCount = 0;
	static FT_Library s_Library;

	static constexpr uint32_t s_ASCII_Start	= 32;
	static constexpr uint32_t s_ASCII_End	= 128;
	static constexpr uint32_t s_GlyphCount = s_ASCII_End - s_ASCII_Start;

	OpenGLFont::OpenGLFont(const std::string& filepath, uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		if (s_FontCount == 0)
			Init();

		s_FontCount++;

		FT_Error error;
		error = FT_New_Face(s_Library, filepath.c_str(), 0, &m_Face);
		QK_ASSERT(error == FT_Err_Ok, "Could not load new font face at path: '" << filepath << "'.");

		error = FT_Set_Pixel_Sizes(m_Face, width, height);
		QK_ASSERT(error == FT_Err_Ok, "Could not set font dimensions.");

		QK_CORE_TRACE("Loading " << s_GlyphCount << " glyphs from font at path: '" << filepath << "'.");

		FT_GlyphSlot g = m_Face->glyph;
		uint32_t w = 0;
		uint32_t h = 0;

		for (uint32_t i = s_ASCII_Start; i < s_ASCII_End; i++)
		{
			if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER))
			{
				QK_CORE_TRACE("Failed to load char: '" << i << "'.");
				continue;
			}

			w += g->bitmap.width;
			h = std::max(h, g->bitmap.rows);
		}

		m_AtlasWidth = w;
		m_AtlasHeight = h;

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		uint32_t x = 0;
		for (uint32_t i = s_ASCII_Start; i < s_ASCII_End; i++)
		{
			if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER))
				continue;

			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

			Character character = {
				{ g->bitmap.width, g->bitmap.rows },
				{ g->bitmap_left, g->bitmap_top },
				{ g->advance.x >> 6, g->advance.y >> 6 },
				(float)x / (float)w
			};

			m_Characters[i] = character;

			x += g->bitmap.width;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		FT_Done_Face(m_Face);
	}

	OpenGLFont::~OpenGLFont()
	{
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

	void OpenGLFont::Init()
	{
		QK_TIME_SCOPE_DEBUG(OpenGLFont::Init);

		FT_Error error = FT_Init_FreeType(&s_Library);
		QK_ASSERT(error == FT_Err_Ok, "Could not initialize freetype!");
	}

	void OpenGLFont::Shutdown()
	{
		QK_TIME_SCOPE_DEBUG(OpenGLFont::Shutdown);

		FT_Done_FreeType(s_Library);
	}
}
