#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Font.h"

#include "UIElement.h"

namespace Quark {

	enum class HorizontalTextAlignment
	{
		Left = 0,
		Right,
		Center,

		Default = Left
	};

	enum class VerticalTextAlignment
	{
		Bottom = 0,
		Top,
		Center,

		Default = Bottom
	};

	struct TextStyle
	{
		HorizontalTextAlignment HorizontalAlign{};
		VerticalTextAlignment   VerticalAlign{};
		Ref<Font>               Font;
		Vec4f                   Color = Vec4f(1.0f);
	};

	class Text : public UIElement
	{
	public:
		Text() = default;
		Text(const TextStyle& style);
		Text(std::string_view text, const TextStyle& style);

		Text& SetValue(std::string_view text);
		Text& SetFont(const Ref<Font>& font);
		Text& SetHorizontalAlignment(HorizontalTextAlignment alignment);
		Text& SetVerticalAlignment(VerticalTextAlignment alignment);

		std::string_view GetValue() const { return m_Value; }
		operator std::string_view() const { return GetValue(); }

		const TextStyle& GetStyle() const { return m_Style; }
		TextStyle& GetStyle() { return m_Style; }

		int32_t GetOriginX() const;
		int32_t GetOriginY() const;

	protected:
		std::string m_Value;
		TextStyle m_Style{};
		int32_t m_Width = 0, m_Height = 0;
	};
}
