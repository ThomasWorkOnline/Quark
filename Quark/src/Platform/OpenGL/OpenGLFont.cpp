#include "qkpch.h"
#include "OpenGLFont.h"

#include "Quark/Renderer/Renderer.h"

#pragma warning(push, 0)
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#pragma warning(pop)

#include <glad/glad.h>

namespace Quark {

	static uint32_t s_FontCount = 0;
	static FT_Library s_Library;

	OpenGLFont::OpenGLFont(std::string_view filepath, uint32_t fontSize)
	{
		QK_PROFILE_FUNCTION();

		if (s_FontCount == 0)
			Init();

		++s_FontCount;

		FT_Error error;
		error = FT_New_Face(s_Library, filepath.data(), 0, &m_Face);
		QK_CORE_RUNTIME_VERIFY(error == FT_Err_Ok, "Could not load new font face at path: '{0}' ({1})", filepath, FT_Error_String(error));

		error = FT_Set_Pixel_Sizes(m_Face, 0, fontSize);
		QK_CORE_RUNTIME_VERIFY(error == FT_Err_Ok, "Could not set font dimensions. ({0}: {1} requested)", FT_Error_String(error), fontSize);

		QK_CORE_TRACE("Loading {0} glyphs from font at path: '{1}'", s_GlyphCount, filepath);

		FT_GlyphSlot g = m_Face->glyph;
		Glyph* glyphPtr = m_Glyphs;

		uint32_t w = 0;
		uint32_t h = 0;

		for (uint8_t i = s_ASCII_Start; i < s_ASCII_End; i++)
		{
			if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER) != FT_Err_Ok)
			{
				*glyphPtr = {};
				QK_CORE_ERROR("Failed to load char: '{0}'", i);
				continue;
			}

			glyphPtr->Size    = { g->bitmap.width, g->bitmap.rows };
			glyphPtr->Bearing = { g->bitmap_left, g->bitmap_top };
			glyphPtr->Advance = { g->advance.x, g->advance.y };
			glyphPtr->OffsetX = w;
			glyphPtr++;

			w += g->bitmap.width + 2;
			h = std::max(h, g->bitmap.rows);
		}

		m_AtlasWidth = w;
		m_AtlasHeight = h;

		QK_CORE_ASSERT(m_AtlasWidth <= Renderer::GetCapabilities().TextureCapabilities.MaxPixelSize
			&& m_AtlasHeight <= Renderer::GetCapabilities().TextureCapabilities.MaxPixelSize, "Font atlas dimensions too large");

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		for (uint8_t i = 0; i < s_GlyphCount; i++)
		{
			if (FT_Load_Char(m_Face, i + s_ASCII_Start, FT_LOAD_RENDER) != FT_Err_Ok)
				continue;

			Glyph& glyph = m_Glyphs[i];
			glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.OffsetX, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		}

		GLint swizzleMask[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	OpenGLFont::~OpenGLFont()
	{
		QK_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);

		FT_Error error = FT_Done_Face(m_Face);
		QK_CORE_ASSERT(error == FT_Err_Ok, "Could not delete font! ({0})", FT_Error_String(error));

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
		return m_Glyphs[charcode - s_ASCII_Start];
	}

	uint32_t OpenGLFont::GetGlyphCount() const
	{
		return static_cast<uint32_t>(s_GlyphCount);
	}

	uint32_t OpenGLFont::GetFontSize() const
	{
		return m_Face->size->metrics.x_ppem;
	}

	std::string_view OpenGLFont::GetStyleName() const
	{
		return m_Face->style_name;
	}

	std::string_view OpenGLFont::GetFamilyName() const
	{
		return m_Face->family_name;
	}

	bool OpenGLFont::operator==(const Texture& other) const
	{
		if (auto o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}

	void OpenGLFont::Init()
	{
		QK_PROFILE_FUNCTION();

		FT_Error error = FT_Init_FreeType(&s_Library);
		QK_CORE_ASSERT(error == FT_Err_Ok, "Could not initialize FreeType! ({0})", FT_Error_String(error));
	}

	void OpenGLFont::Shutdown()
	{
		QK_PROFILE_FUNCTION();

		FT_Error error = FT_Done_FreeType(s_Library);
		QK_CORE_ASSERT(error == FT_Err_Ok, "Could not shutdown FreeType! ({0})", FT_Error_String(error));
	}
}
