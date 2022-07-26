#include "qkpch.h"
#include "TextInput.h"

#include <iostream>

namespace Quark {

	TextInput::TextInput(const Ref<Font>& font,
		HorizontalTextAlignment hAlign, VerticalTextAlignment vAlign)
	{
		m_RenderTraits.FontStyle = font;
		m_RenderTraits.HAlign    = hAlign;
		m_RenderTraits.VAlign    = vAlign;
	}

	void TextInput::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(TextInput::OnKeyPressed));
		dispatcher.Dispatch<KeyTypedEvent>(ATTACH_EVENT_FN(TextInput::OnKeyTyped));
	}

	void TextInput::SetFont(const Ref<Font>& font)
	{
		m_RenderTraits.FontStyle = font;
		m_RenderTraits.CalculateLabelDimensions(m_Input);
	}

	bool TextInput::OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case KeyCode::Backspace:
			if (m_Input.size() > 0)
			{
				m_Input.pop_back();
				m_RenderTraits.CalculateLabelDimensions(m_Input);
			}
			break;
		}

		return false;
	}

	bool TextInput::OnKeyTyped(KeyTypedEvent& e)
	{
		uint16_t keyCode = static_cast<uint16_t>(e.GetKeyCode());
		m_Input.push_back(static_cast<char>(keyCode));
		m_RenderTraits.CalculateLabelDimensions(m_Input);

		return false;
	}
}
