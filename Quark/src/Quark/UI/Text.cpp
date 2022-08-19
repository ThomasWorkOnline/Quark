#include "qkpch.h"
#include "Text.h"

namespace Quark {

	Text::Text(const TextStyle& style)
		: m_Style(style)
	{
	}

	Text::Text(std::string_view text, const TextStyle& style)
		: m_Value(text), m_Style(style)
	{
	}

	Text& Text::SetValue(std::string_view text)
	{
		m_Value = text;
		return *this;
	}

	Text& Text::SetFont(const Ref<Font>& font)
	{
		m_Style.Font = font;
		return *this;
	}

	Text& Text::SetHorizontalAlignment(HorizontalTextAlignment alignment)
	{
		m_Style.HorizontalAlign = alignment;
		return *this;
	}

	Text& Text::SetVerticalAlignment(VerticalTextAlignment alignment)
	{
		m_Style.VerticalAlign = alignment;
		return *this;
	}

	int32_t Text::GetOriginX() const
	{
		switch (m_Style.HorizontalAlign)
		{
			case HorizontalTextAlignment::Left:   return -m_Width;
			case HorizontalTextAlignment::Right:  return 0;
			case HorizontalTextAlignment::Center: return -m_Width / 2;

			QK_ASSERT_NO_DEFAULT("No alignment mode was specified");
		}

		return 0;
	}

	int32_t Text::GetOriginY() const
	{
		switch (m_Style.VerticalAlign)
		{
			case VerticalTextAlignment::Bottom: return 0;
			case VerticalTextAlignment::Top:    return m_Height;
			case VerticalTextAlignment::Center: return m_Height / 2;

			QK_ASSERT_NO_DEFAULT("No alignment mode was specified");
		}

		return 0;
	}
}
