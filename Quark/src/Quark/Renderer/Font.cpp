#include "qkpch.h"
#include "Font.h"

#pragma warning(push, 0)
#	include <ft2build.h>
#	include FT_FREETYPE_H
#	include FT_GLYPH_H
#pragma warning(pop)

#include "GraphicsAPI.cpp"

static uint32_t s_FontCount = 0;
static FT_Library s_Library;

namespace Quark {

	static void Init()
	{
		QK_PROFILE_FUNCTION();

		FT_Error error = FT_Init_FreeType(&s_Library);
		QK_CORE_ASSERT(error == FT_Err_Ok, "Could not initialize FreeType! ({0})", FT_Error_String(error));
	}

	static void Shutdown()
	{
		QK_PROFILE_FUNCTION();

		FT_Error error = FT_Done_FreeType(s_Library);
		QK_CORE_ASSERT(error == FT_Err_Ok, "Could not shutdown FreeType! ({0})", FT_Error_String(error));
	}

	Font::Font(std::string_view filepath, uint32_t fontSize)
	{
		if (s_FontCount == 0)
			Init();

		++s_FontCount;

		FT_Error error;
		error = FT_New_Face(s_Library, filepath.data(), 0, &m_Face);
		Verify(error == FT_Err_Ok, "Could not load new font face at path: '{0}' ({1})", filepath, FT_Error_String(error));

		error = FT_Set_Pixel_Sizes(m_Face, 0, fontSize);
		Verify(error == FT_Err_Ok, "Could not set font dimensions. ({0}: {1} requested)", FT_Error_String(error), fontSize);

		QK_CORE_TRACE("Loading {0} glyphs from font at path: '{1}'", GlyphCount, filepath);

		FT_GlyphSlot g = m_Face->glyph;
		Glyph* glyphPtr = m_Glyphs;

		uint32_t w = 0;
		uint32_t h = 0;

		for (uint8_t i = 0; i < GlyphCount; i++)
		{
			if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER) != FT_Err_Ok)
			{
				*glyphPtr = {};
				QK_CORE_ERROR("Failed to load char: '{0}'", i);
				continue;
			}

			glyphPtr->Size = { g->bitmap.width, g->bitmap.rows };
			glyphPtr->Bearing = { g->bitmap_left, g->bitmap_top };
			glyphPtr->Advance = { g->advance.x, g->advance.y };
			glyphPtr->OffsetX = w;
			glyphPtr++;

			w += g->bitmap.width + 2;
			h = std::max(h, g->bitmap.rows);
		}

		m_AtlasWidth = w;
		m_AtlasHeight = h;
	}

	Font::~Font()
	{
		--s_FontCount;

		if (s_FontCount == 0)
			Shutdown();
	}

	const Glyph& Font::GetGlyph(uint8_t charcode) const
	{
		return m_Glyphs[charcode];
	}

	uint32_t Font::GetGlyphCount() const
	{
		return static_cast<uint32_t>(GlyphCount);
	}

	uint32_t Font::GetFontSize() const
	{
		return m_Face->size->metrics.x_ppem;
	}

	std::string_view Font::GetStyleName() const
	{
		return m_Face->style_name;
	}

	std::string_view Font::GetFamilyName() const
	{
		return m_Face->family_name;
	}

	Scope<Font> Font::Create(std::string_view filepath, uint32_t fontSize)
	{
		return s_GraphicsAPI->CreateFont(filepath, fontSize);
	}

	static size_t GetHashedName(std::string_view name)
	{
		return std::hash<std::string_view>()(name);
	}

	Ref<Font> FontLibrary::Load(std::string_view name, std::string_view filepath, uint32_t fontSize)
	{
		Ref<Font> font = Font::Create(filepath, fontSize);
		Add(name, font);
		return font;
	}

	Ref<Font> FontLibrary::Get(std::string_view name) const
	{
		QK_CORE_ASSERT(Exists(name), "Font not found!");
		return m_Fonts.at(GetHashedName(name));
	}

	void FontLibrary::Add(std::string_view name, const Ref<Font>& font)
	{
		QK_CORE_ASSERT(!Exists(name), "Font already exists!");
		m_Fonts[GetHashedName(name)] = font;
	}

	bool FontLibrary::Exists(std::string_view name) const
	{
		return m_Fonts.find(GetHashedName(name)) != m_Fonts.end();
	}
}
