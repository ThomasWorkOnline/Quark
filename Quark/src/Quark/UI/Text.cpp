#include "qkpch.h"
#include "Text.h"

namespace Quark {

	Text::Text(std::string text, Ref<Font> font, const Vec4f& color, HorizontalTextAlignment hAlign, VerticalTextAlignment vAlign)
		: m_Text(std::move(text))
	{
		m_RenderTraits.FontStyle = std::move(font);
		m_RenderTraits.Color     = color;
		m_RenderTraits.HAlign    = hAlign;
		m_RenderTraits.VAlign   = vAlign;
		m_RenderTraits.CalculateLabelDimensions(m_Text);
	}

	void Text::SetText(std::string text)
	{
		m_Text = std::move(text);
		m_RenderTraits.CalculateLabelDimensions(m_Text);
	}

	void Text::SetFont(Ref<Font> font)
	{
		m_RenderTraits.FontStyle = std::move(font);
		m_RenderTraits.CalculateLabelDimensions(m_Text);
	}
}
