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

	class Text : public UIElement
	{
	public:
		Text() = default;
		Text(std::string text, const Ref<Font>& font, const glm::vec4& color,
			HorizontalTextAlignment hAlign = HorizontalTextAlignment::Default, VerticalTextAlignment vAlign = VerticalTextAlignment::Default);

		std::string_view GetString() const { return m_Text; }
		void SetString(std::string text);

		const Ref<Font>& GetFont() const { return m_Font; }
		void SetFont(const Ref<Font>& font);

		const glm::vec4& GetColor() const { return m_Color; }
		void SetColor(const glm::vec4& color) { m_Color = color; }

		HorizontalTextAlignment GetHorizontalAlignment() const { return m_HAlign; }
		void SetHorizontalAlignment(HorizontalTextAlignment align) { m_HAlign = align; }

		VerticalTextAlignment GetVerticalAlignment() const { return m_VAlign; }
		void SetVerticalAlignment(VerticalTextAlignment align) { m_VAlign = align; }

		glm::ivec2 GetOrigin() const { return { GetOriginX(), GetOriginY() }; }
		int32_t GetOriginX() const;
		int32_t GetOriginY() const;

	private:
		void CalculateLabelDimensions();

	private:
		std::string m_Text;
		Ref<Font> m_Font;
		glm::vec4 m_Color = glm::vec4(1.0f);

		int32_t m_Width = 0, m_Height = 0;

		HorizontalTextAlignment m_HAlign{};
		VerticalTextAlignment m_VAlign{};
	};
}
