#include "qkpch.h"
#include "TextInput.h"

namespace Quark {

	TextInput::TextInput(const TextStyle& style)
		: Text(style)
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
				if (m_Cursor > 0 && m_Cursor <= m_Value.size())
				{
					m_Cursor--;
					m_Value.erase(m_Cursor, 1);
				}
				break;
			}
			case KeyCode::Delete:
			{
				if (m_Cursor >= 0 && m_Cursor < m_Value.size())
				{
					m_Value.erase(m_Cursor, 1);
				}
				break;
			}
			case KeyCode::Enter:
			{
				m_Value.insert(m_Value.begin() + m_Cursor++, '\n');
				break;
			}
			case KeyCode::Tab:
			{
				m_Value.insert(m_Value.begin() + m_Cursor++, '\t');
				break;
			}
			case KeyCode::Left:
			{
				if (m_Cursor > 0) m_Cursor--;
				break;
			}
			case KeyCode::Right:
			{
				if (m_Cursor < m_Value.size()) m_Cursor++;
				break;
			}
			case KeyCode::Home:
			{
				m_Cursor = 0;
				break;
			}
			case KeyCode::End:
			{
				m_Cursor = m_Value.size();
				break;
			}
		}
	}

	void TextInput::OnKeyTyped(KeyTypedEvent& e)
	{
		auto charcode = static_cast<uint16_t>(e.GetKeyCode());
		if (charcode < 256)
		{
			m_Value.insert(m_Value.begin() + m_Cursor++, (char)charcode);
		}
	}
}
