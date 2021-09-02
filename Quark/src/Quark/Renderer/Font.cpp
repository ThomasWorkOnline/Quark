#include "Font.h"

#include "../Core/Core.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Quark {

	Ref<Font> Font::Create(const std::string& filepath)
	{
		return CreateRef<Font>();
	}

	Ref<Font> FontLibrary::Load(const std::string& name, const std::string& filepath)
	{
		Ref<Font> font = Font::Create(filepath);
		Add(font, name);
		return font;
	}

	void FontLibrary::Add(const Ref<Font>& font, const std::string& name)
	{
		if (!Exists(name))
			m_Fonts[name] = font;
		else
			QK_CORE_WARN("Font already exists! It was not added");
	}

	const Ref<Font>& FontLibrary::Get(const std::string& name)
	{
		if (!Exists(name))
			QK_FATAL("Font not found!");

		return m_Fonts[name];
	}

	bool FontLibrary::Exists(const std::string& name) const
	{
		return m_Fonts.find(name) != m_Fonts.end();
	}

	static FT_Library s_Library;

	void FontLoader::Initialize()
	{
		QK_TIME_SCOPE_DEBUG(FontEngine::Initialize);

		FT_Error error = FT_Init_FreeType(&s_Library);
		QK_ASSERT(error == FT_Err_Ok, "Could not initialize freetype!");
	}

	void FontLoader::Dispose()
	{
		delete s_Library;
	}
}
