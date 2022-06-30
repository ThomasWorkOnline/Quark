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
}
