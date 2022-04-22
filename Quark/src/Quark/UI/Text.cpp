#include "qkpch.h"
#include "Text.h"

namespace Quark {

	Text::Text(std::string text, const Ref<Font>& font, const glm::vec4& color, HorizontalTextAlignment hAlign, VerticalTextAlignment vAlign)
		: m_Text(std::move(text))
	{
		m_RenderTraits.FontStyle = font;
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

	void Text::SetFont(const Ref<Font>& font)
	{
		m_RenderTraits.FontStyle = font;
		m_RenderTraits.CalculateLabelDimensions(m_Text);
	}
}
