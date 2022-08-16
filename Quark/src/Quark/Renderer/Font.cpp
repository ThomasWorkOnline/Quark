#include "qkpch.h"
#include "Font.h"

#include "GraphicsAPI.h"

namespace Quark {

	Scope<Font> Font::Create(std::string_view filepath, uint32_t fontSize)
	{
		return GraphicsAPI::Instance->CreateFont(filepath, fontSize);
	}

	static size_t GetHashedName(std::string_view name)
	{
		return std::hash<std::string_view>()(name);
	}

	Ref<Font> FontLibrary::Load(std::string_view name, std::string_view filepath, uint32_t fontSize)
	{
		Ref<Font> font(Font::Create(filepath, fontSize));
		Add(font, name);
		return font;
	}

	Ref<Font> FontLibrary::Get(std::string_view name) const
	{
		QK_CORE_ASSERT(Exists(name), "Font not found!");
		return m_Fonts.at(GetHashedName(name));
	}

	void FontLibrary::Add(Ref<Font> font, std::string_view name)
	{
		QK_CORE_ASSERT(!Exists(name), "Font already exists!");
		m_Fonts[GetHashedName(name)] = std::move(font);
	}

	bool FontLibrary::Exists(std::string_view name) const
	{
		return m_Fonts.find(GetHashedName(name)) != m_Fonts.end();
	}
}
