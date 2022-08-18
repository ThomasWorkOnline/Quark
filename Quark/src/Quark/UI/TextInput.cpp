#include "qkpch.h"
#include "TextInput.h"

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
	}

	void TextInput::OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
			case KeyCode::Backspace:
			{
				if (m_Input.size() > 0)
					m_Input.pop_back();
			} break;
		}
	}

	void TextInput::OnKeyTyped(KeyTypedEvent& e)
	{
		m_Input.push_back(static_cast<char>(e.GetKeyCode()));
	}
}
