#pragma once

#include "Event.h"
#include "Quark/Core/Input.h"

namespace Quark {

    class MouseEvent : public Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input);

    protected:
        MouseEvent() = default;
    };

    class MouseMovedEvent : public MouseEvent
    {
    public:
    	MouseMovedEvent(float x, float y, float xOffset, float yOffset)
    		: m_MouseX(x), m_MouseY(y), m_XOffset(xOffset), m_YOffset(yOffset) {}

    	float GetX() const { return m_MouseX; }
    	float GetY() const { return m_MouseY; }

        float GetXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }

    	std::string ToString() const override
    	{
    		std::stringstream ss;
    		ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
            ss << " (" << m_XOffset << ", " << m_YOffset << ")";
    		return ss.str();
    	}

        EVENT_CLASS_TYPE(EventType::MouseMoved);

    private:
    	float m_MouseX, m_MouseY;
        float m_XOffset, m_YOffset;
    };

    class MouseScrolledEvent : public MouseEvent
    {
    public:
    	MouseScrolledEvent(float xOffset, float yOffset)
    		: m_XOffset(xOffset), m_YOffset(yOffset) {}

    	float GetXOffset() const { return m_XOffset; }
    	float GetYOffset() const { return m_YOffset; }

    	std::string ToString() const override
    	{
    		std::stringstream ss;
    		ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
    		return ss.str();
    	}

        EVENT_CLASS_TYPE(EventType::MouseScrolled);

    private:
    	float m_XOffset, m_YOffset;
    };

    class MouseButtonEvent : public MouseEvent
    {
    public:
    	inline MouseCode GetMouseButton() const { return m_Button; }

    protected:
    	MouseButtonEvent(MouseCode button)
    		: m_Button(button) {}

    	MouseCode m_Button;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
    	MouseButtonPressedEvent(MouseCode button)
    		: MouseButtonEvent(button) {}

    	std::string ToString() const override
    	{
    		std::stringstream ss;
    		ss << "MouseButtonPressedEvent: " << m_Button;
    		return ss.str();
    	}

        EVENT_CLASS_TYPE(EventType::MouseButtonPressed);
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
    	MouseButtonReleasedEvent(MouseCode button)
    		: MouseButtonEvent(button) {}

    	std::string ToString() const override
    	{
    		std::stringstream ss;
    		ss << "MouseButtonReleasedEvent: " << m_Button;
    		return ss.str();
    	}

        EVENT_CLASS_TYPE(EventType::MouseButtonReleased);
    };
}
