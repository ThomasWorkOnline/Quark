#pragma once

#include "Event.h"

namespace Quark {

    class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(EventType::AppTick);
		EVENT_CLASS_CATEGORY(EventCategory::Application);
	};

    class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(EventType::AppUpdate);
		EVENT_CLASS_CATEGORY(EventCategory::Application);
	};

    class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(EventType::AppRender);
		EVENT_CLASS_CATEGORY(EventCategory::Application);
	};

	class ViewportResizedEvent : public Event
	{
	public:
		ViewportResizedEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height)
		{
		}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ViewportResizedEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::ViewportResized);
		EVENT_CLASS_CATEGORY(EventCategory::Application);

	private:
		uint32_t m_Width, m_Height;
	};
}
