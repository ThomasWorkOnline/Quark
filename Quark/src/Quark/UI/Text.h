#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Font.h"
#include "Quark/Renderer/TextRendererTraits.h"

#include "UIElement.h"

namespace Quark {

	class Text final : public UIElement
	{
	public:
		Text() = default;
		Text(std::string text, Ref<Font> font, const Vec4f& color,
			HorizontalTextAlignment hAlign = HorizontalTextAlignment::Default, VerticalTextAlignment vAlign = VerticalTextAlignment::Default);

		std::string_view GetText() const { return m_Text; }
		void SetText(std::string text);
		void SetFont(Ref<Font> font);
		void SetHorizontalAlignment(HorizontalTextAlignment align) { m_RenderTraits.HAlign = align; }
		void SetVerticalAlignment(VerticalTextAlignment align) { m_RenderTraits.VAlign = align; }

		const TextRenderTraits& GetRenderTraits() const { return m_RenderTraits; }

	private:
		std::string m_Text;
		TextRenderTraits m_RenderTraits{};
	};
}
