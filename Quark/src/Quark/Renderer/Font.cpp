#include "Font.h"

#include "RenderingAPI.h"

// Include all supported API's font implementations
#include "../../Platform/OpenGL/OpenGLFont.h"

namespace Quark {

	Ref<Font> Font::Create(const std::string& filepath)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return Resource::Create<OpenGLFont>(filepath);
		case RenderingAPI::API::None:
			QK_FATAL("Rendering API not supported");
		}
		return nullptr;
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
}
