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

	private:
		void OnKeyPressed(KeyPressedEvent& e);
		void OnKeyTyped(KeyTypedEvent& e);

	private:
		struct TextSelection
		{
			std::string::iterator Start;
			std::string::iterator End;
		};
	};
}
