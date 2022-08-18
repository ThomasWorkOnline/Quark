#pragma once

#include "Quark/Core/Core.h"

#include "Quark/Event/KeyEvent.h"
#include "Quark/Renderer/Font.h"
#include "Quark/Renderer/TextRendererTraits.h"
#include "Quark/UI/UIElement.h"

namespace Quark {

	class TextInput final : public UIElement
	{
	public:
		TextInput() = default;
		TextInput(const Ref<Font>& font, HorizontalTextAlignment hAlign = HorizontalTextAlignment::Right, VerticalTextAlignment vAlign = VerticalTextAlignment::Default);

		void OnEvent(Event& e);

		void SetFont(const Ref<Font>& font);
		void SetHorizontalAlignment(HorizontalTextAlignment align) { m_RenderTraits.HAlign = align; }
		void SetVerticalAlignment(VerticalTextAlignment align) { m_RenderTraits.VAlign = align; }

		std::string_view GetText() const { return m_Input; }
		const TextRenderTraits& GetRenderTraits() const { return m_RenderTraits; }

	private:
		void OnKeyPressed(KeyPressedEvent& e);
		void OnKeyTyped(KeyTypedEvent& e);

	private:
		struct TextSelection
		{
			std::string::iterator Start;
			std::string::iterator End;
		};

		TextRenderTraits m_RenderTraits{};

		// Only supports ASCII strings
		std::string m_Input;
	};
}
