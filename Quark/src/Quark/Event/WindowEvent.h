#pragma once

#include "Event.h"

namespace Quark {

	class WindowEvent : public Event
	{
	protected:
		WindowEvent() = default;

		EVENT_CLASS_CATEGORY(EventCategory::Application);
	};

	class WindowMovedEvent : public WindowEvent
	{
	public:
		WindowMovedEvent(int32_t xpos, int32_t ypos, int32_t xOffset, int32_t yOffset)
			: m_Xpos(xpos), m_YPos(ypos), m_XOffset(xOffset), m_YOffset(yOffset)
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << m_Xpos << ", " << m_YPos;
			ss << " (" << m_XOffset << ", " << m_YOffset << ")";
			return ss.str();
		}

		int32_t GetX() const { return m_Xpos; }
		int32_t GetY() const { return m_YPos; }

		int32_t GetXOffset() const { return m_XOffset; }
		int32_t GetYOffset() const { return m_YOffset; }

		EVENT_CLASS_TYPE(EventType::WindowMoved);

	private:
		int32_t m_Xpos, m_YPos;
		int32_t m_XOffset, m_YOffset;
	};

	class WindowResizedEvent : public WindowEvent
	{
	public:
		WindowResizedEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height)
		{
		}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::WindowResized);

	private:
		uint32_t m_Width, m_Height;
	};

	class WindowMaximizedEvent : public WindowEvent
	{
	public:
		WindowMaximizedEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowMaximized);
	};

	class WindowMinimizedEvent : public WindowEvent
	{
	public:
		WindowMinimizedEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowMinimized);
	};

	class WindowRestoredEvent : public WindowEvent
	{
	public:
		WindowRestoredEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowRestored);
	};

	class WindowClosedEvent : public WindowEvent
	{
	public:
		WindowClosedEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowClosed);
	};

	class WindowFocusedEvent : public WindowEvent
	{
	public:
		WindowFocusedEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowFocused);
	};

	class WindowLostFocusEvent : public WindowEvent
	{
	public:
		WindowLostFocusEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowLostFocus);
	};
}
