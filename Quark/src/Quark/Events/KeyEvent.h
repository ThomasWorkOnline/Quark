#pragma once

#include "Event.h"
#include "Quark/Core/Input.h"

namespace Quark {

    class KeyEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }
		ModifierKey GetModifierKeys() const { return m_Mods; }

		bool WasModifierKeyPressed(ModifierKey key) { return (uint8_t)m_Mods & (uint8_t)key; }

		EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input);

	protected:
		KeyEvent(KeyCode keycode, ModifierKey mods)
			: m_KeyCode(keycode), m_Mods(mods) {}

		KeyCode m_KeyCode;
		ModifierKey m_Mods;
	};

    class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode keycode, ModifierKey mods, int32_t repeatCount)
			: KeyEvent(keycode, mods), m_RepeatCount(repeatCount) {}

		int32_t GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::KeyPressed);

	private:
		int32_t m_RepeatCount;
	};

    class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyCode keycode, ModifierKey mods)
			: KeyEvent(keycode, mods) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::KeyReleased);
	};

    class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(KeyCode keycode)
			: KeyEvent(keycode, ModifierKey::None) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::KeyTyped);
	};
}
