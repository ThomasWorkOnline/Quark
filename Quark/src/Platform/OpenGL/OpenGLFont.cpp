#include "qkpch.h"
#include "OpenGLFont.h"

#include "Quark/Renderer/Renderer.h"

#pragma warning(push, 0)
#	include <ft2build.h>
#	include FT_FREETYPE_H
#	include FT_GLYPH_H
#pragma warning(pop)

#include <glad/glad.h>

namespace Quark {

	OpenGLFont::OpenGLFont(std::string_view filepath, uint32_t fontSize)
		: Font(filepath, fontSize)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_AtlasWidth <= Renderer::GetCapabilities().Texture.Max2DSize
			&& m_AtlasHeight <= Renderer::GetCapabilities().Texture.Max2DSize,
			"Font atlas dimensions too large: see Renderer::GetCapabilities() for more info");

		m_Target = GL_TEXTURE_2D;
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_AtlasWidth, m_AtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

		FT_GlyphSlot g = m_Face->glyph;
		for (uint8_t i = 0; i < GlyphCount; i++)
		{
			if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER) != FT_Err_Ok || (g->bitmap.width == 0 || g->bitmap.rows == 0))
				continue;

			const Glyph& glyph = m_Glyphs[i];
			glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.OffsetX, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		}

		GLint swizzleMask[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

		QK_CORE_ASSERT(glIsTexture(m_RendererID), "Font atlas is incomplete!");
		QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	OpenGLFont::~OpenGLFont()
	{
		QK_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);

		FT_Error error = FT_Done_Face(m_Face);
		QK_CORE_ASSERT(error == FT_Err_Ok, "Could not delete font! ({0})", FT_Error_String(error));
	}

	bool OpenGLFont::operator==(const Texture& other) const
	{
		if (auto o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}
}
