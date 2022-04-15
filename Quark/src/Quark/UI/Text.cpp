#include "Text.h"

namespace Quark {

	Text::Text(std::string text, const Ref<Font>& font, const glm::vec4& color, HorizontalTextAlignment hAlign, VerticalTextAlignment vAlign)
		: m_Text(std::move(text)), m_Font(font), m_Color(color), m_HAlign(hAlign), m_VAlign(vAlign)
	{
		CalculateLabelDimensions();
	}

	void Text::SetString(std::string text)
	{
		m_Text = std::move(text);
		CalculateLabelDimensions();
	}

	void Text::SetFont(const Ref<Font>& font)
	{
		m_Font = font;
		CalculateLabelDimensions();
	}

	int32_t Text::GetOriginX() const
	{
		switch (m_HAlign)
		{
			case Quark::HorizontalTextAlignment::Left:   return -m_Width;
			case Quark::HorizontalTextAlignment::Right:  return 0;
			case Quark::HorizontalTextAlignment::Center: return -m_Width / 2.0f;
			default:
				QK_CORE_ASSERT(false, "No alignment mode was specified.");
				return 0.0f;
		}
	}

	int32_t Text::GetOriginY() const
	{
		switch (m_VAlign)
		{
			case Quark::VerticalTextAlignment::Bottom: return 0.0f;
			case Quark::VerticalTextAlignment::Top:    return m_Height;
			case Quark::VerticalTextAlignment::Center: return m_Height / 2.0f;
			default:
				QK_CORE_ASSERT(false, "No alignment mode was specified.");
				return 0.0f;
		}
	}

	void Text::CalculateLabelDimensions()
	{
		int32_t width = 0, height = 0;
		for (auto it = m_Text.begin(); it != m_Text.end(); it++)
		{
			auto& g = m_Font->GetGlyph(*it);
			width += g.Advance.x >> 6;
			height = std::max(height, g.Size.y);
		}

		m_Width = width;
		m_Height = height;
	}
}
