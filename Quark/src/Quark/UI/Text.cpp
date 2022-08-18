#include "qkpch.h"
#include "Text.h"

namespace Quark {

	Text::Text(const std::string& text, const Ref<Font>& font, const Vec4f& color, HorizontalTextAlignment hAlign, VerticalTextAlignment vAlign)
		: m_Text(text)
	{
		m_RenderTraits.FontStyle = font;
		m_RenderTraits.Color     = color;
		m_RenderTraits.HAlign    = hAlign;
		m_RenderTraits.VAlign    = vAlign;
	}

	void Text::SetText(const std::string& text)
	{
		m_Text = text;
	}

	void Text::SetFont(const Ref<Font>& font)
	{
		m_RenderTraits.FontStyle = font;
	}
}
