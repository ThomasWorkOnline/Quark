#include "qkpch.h"
#include "Font.h"

#include "GraphicsAPI.h"

namespace Quark {

	Ref<Font> Font::Create(std::string_view filepath, uint32_t fontSize)
	{
		return GraphicsAPI::Instance->CreateFont(filepath, fontSize);
	}

	Ref<Font> FontLibrary::Load(const std::string& name, std::string_view filepath, uint32_t fontSize)
	{
		Ref<Font> font = Font::Create(filepath, fontSize);
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
		QK_CORE_ASSERT(Exists(name), "Font not found!");
		return m_Fonts[name];
	}

	bool FontLibrary::Exists(const std::string& name) const
	{
		return m_Fonts.find(name) != m_Fonts.end();
	}
}
