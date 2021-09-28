#pragma once

#include "../Core/Core.h"

#include "../Renderer/Font.h"

#include "UIElement.h"

namespace Quark {

	enum class HorizontalTextAlignment
	{
		Left = 0,
		Right,
		Center,
		Justify,

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
		Text(const std::string& text, const Ref<Font>& font, const glm::vec4& color, float width, float height,
			HorizontalTextAlignment hAlign = HorizontalTextAlignment::Default, VerticalTextAlignment vAlign = VerticalTextAlignment::Default);

		const std::string& GetString() const { return m_Text; }
		void SetString(const std::string& text) { m_Text = text; }

		const Ref<Font>& GetFont() const { return m_Font; }
		void SetFont(const Ref<Font>& font) { m_Font = font; }

		const glm::vec4& GetColor() const { return m_Color;  }
		void SetColor(const glm::vec4& color) { m_Color = color; }

		glm::vec2 GetSize() const { return { m_Width, m_Height }; }
		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }

		glm::vec2 GetOrigin() const { return { GetOriginX(), GetOriginY() }; }
		float GetOriginX() const;
		float GetOriginY() const;

	private:
		std::string m_Text;
		Ref<Font> m_Font;
		glm::vec4 m_Color;
		float m_Width, m_Height;

		HorizontalTextAlignment m_HAlign;
		VerticalTextAlignment m_VAlign;
	};
}
