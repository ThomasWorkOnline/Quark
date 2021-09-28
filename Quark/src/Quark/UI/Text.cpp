#include "Text.h"

namespace Quark {

	Text::Text(const std::string& text, const Ref<Font>& font, const glm::vec4& color, float width, float height, HorizontalTextAlignment hAlign, VerticalTextAlignment vAlign)
		: m_Text(text), m_Font(font), m_Color(color), m_Width(width), m_Height(height), m_HAlign(hAlign), m_VAlign(vAlign)
	{
	}

	float Text::GetOriginX() const
	{
		switch (m_HAlign)
		{
		case Quark::HorizontalTextAlignment::Left:
			return 0.0f;
		case Quark::HorizontalTextAlignment::Right:
			return m_Width;
		case Quark::HorizontalTextAlignment::Center:
			return m_Width / 2.0f;
		case Quark::HorizontalTextAlignment::Justify:
			QK_ASSERT(false, "Justify mode not implemented yet.");
			return 0.0f;
		default:
			QK_ASSERT(false, "No alignment mode was specified.");
			return 0.0f;
		}
	}

	float Text::GetOriginY() const
	{
		switch (m_VAlign)
		{
		case Quark::VerticalTextAlignment::Bottom:
			return 0.0f;
		case Quark::VerticalTextAlignment::Top:
			return m_Height;
		case Quark::VerticalTextAlignment::Center:
			return m_Height / 2.0f;
		default:
			QK_ASSERT(false, "No alignment mode was specified.");
			return 0.0f;
		}
	}
}
