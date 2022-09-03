#include "qkpch.h"
#include "TextInput.h"

namespace Quark {

	TextInput::TextInput(const TextStyle& style) : Text(style)
	{
	}

	void TextInput::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(TextInput::OnKeyPressed));
		dispatcher.Dispatch<KeyTypedEvent>(ATTACH_EVENT_FN(TextInput::OnKeyTyped));
	}

	void TextInput::OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
			case KeyCode::Backspace:
			{
				if (m_Value.size() > 0)
					m_Value.pop_back();
				break;
			}
			case KeyCode::Enter:
			{
				m_Value.push_back('\n');
				break;
			}
		}
	}

	void TextInput::OnKeyTyped(KeyTypedEvent& e)
	{
		m_Value.push_back(static_cast<char>(e.GetKeyCode()));
	}
}
