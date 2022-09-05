#pragma once

#include "Quark/Core/Core.h"

#include <functional>
#include <ostream>
#include <sstream>
#include <type_traits>

namespace Quark {

	enum class EventType
	{
		None = 0,
		WindowClosed, WindowResized, WindowFocused, WindowLostFocus, WindowMoved, WindowMaximized, WindowMinimized, WindowRestored,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum class EventCategory : uint8_t
	{
		None        = 0,
		Application = BIT(0),
		Input       = BIT(1),
		Keyboard    = BIT(2),
		Mouse       = BIT(3),
		MouseButton = BIT(4)
	};

	constexpr EventCategory operator &(EventCategory lhs, EventCategory rhs)
	{
		return static_cast<EventCategory>(
			static_cast<std::underlying_type_t<EventCategory>>(lhs) &
			static_cast<std::underlying_type_t<EventCategory>>(rhs)
		);
	}

	constexpr EventCategory operator |(EventCategory lhs, EventCategory rhs)
	{
		return static_cast<EventCategory>(
			static_cast<std::underlying_type_t<EventCategory>>(lhs) |
			static_cast<std::underlying_type_t<EventCategory>>(rhs)
		);
	}

	constexpr EventCategory operator ^(EventCategory lhs, EventCategory rhs)
	{
		return static_cast<EventCategory>(
			static_cast<std::underlying_type_t<EventCategory>>(lhs) ^
			static_cast<std::underlying_type_t<EventCategory>>(rhs)
		);
	}

	constexpr EventCategory operator ~(EventCategory lhs)
	{
		return static_cast<EventCategory>(~static_cast<std::underlying_type_t<EventCategory>>(lhs));
	}

	// To be defined in event declaration
#define EVENT_CLASS_TYPE(type)  static constexpr EventType GetStaticType() { return type; } \
								static constexpr const char* GetStaticName() { return #type; } \
								virtual EventType GetType() const override { return GetStaticType(); } \
								virtual const char* GetName() const override { return GetStaticName(); }

#define EVENT_CLASS_CATEGORY(category) virtual EventCategory GetCategoryFlags() const override { return category; }

#define EVENT_HANDLER_RETURN_TYPE(x, args) decltype(x(args))
	
	class Event
	{
	public:
		bool Handled = false;

		// Virtual methods
		virtual EventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual EventCategory GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return static_cast<bool>(GetCategoryFlags() & category);
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template<typename T, typename Func>
		bool Dispatch(Func&& func)
		{
			if (m_Event.GetType() == T::GetStaticType())
			{
				if constexpr (std::is_same_v<EVENT_HANDLER_RETURN_TYPE(func, (T&)m_Event), bool>)
				{
					m_Event.Handled = func((T&)m_Event);
				}
				else if constexpr (std::is_same_v<EVENT_HANDLER_RETURN_TYPE(func, (T&)m_Event), void>)
				{
					func((T&)m_Event);
				}
				else
				{
					static_assert(false, "Event handler return value must be of type boolean or void");
				}

				return true;
			}

			return false;
		}

	private:
		Event& m_Event;
	};
}
