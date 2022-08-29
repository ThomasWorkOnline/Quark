#include "qkpch.h"
#include "Font.h"

#include "GraphicsAPI.cpp"

namespace Quark {

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
