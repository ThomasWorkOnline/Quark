#pragma once

#include "Quark/Core/Core.h"

#include "Quark/Event/KeyEvent.h"
#include "Quark/Renderer/Font.h"
#include "Quark/UI/Text.h"

namespace Quark {

	class TextInput final : public Text
	{
	public:
		TextInput() = default;
		TextInput(const TextStyle& style);

		void OnEvent(Event& e);

		size_t GetCursor() const { return m_Cursor; }
		char   GetCharAtCursor() const { return m_Value[m_Cursor]; }

	private:
		void OnKeyPressed(KeyPressedEvent& e);
		void OnKeyTyped(KeyTypedEvent& e);

	private:
		struct TextSelection
		{
			size_t Begin;
			size_t End;
		};

		size_t m_Cursor{};
	};
}
