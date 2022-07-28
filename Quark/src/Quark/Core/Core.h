#pragma once

// Platform detection
#include "Config.h"
#include "Platform.h"
#include "Tweaks.h"

#include <functional>
#include <memory>

#define ATTACH_EVENT_FN(fn) [&](auto&&... args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }
#define BIT(x) (1 << x)

namespace Quark {

	template<typename T, typename Deleter = std::default_delete<T>>
	using Scope = std::unique_ptr<T, Deleter>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename Deleter, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::shared_ptr<T>(new T(std::forward<Args>(args)...), Deleter());
	}

	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	struct NonCopyable
	{
		NonCopyable() = default;
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator=(const NonCopyable&) = delete;
	};

	struct NonMovable
	{
		NonMovable() = default;
		NonMovable(NonMovable&&) = delete;
		NonMovable& operator=(NonMovable&&) = delete;
	};

	template<typename T>
	class Singleton : public NonCopyable, public NonMovable
	{
	public:
		using ValueType     = T;
		using PointerType   = T*;
		using ReferenceType = T&;

	public:
		static inline ReferenceType Get() { return *s_Instance; }

	protected:
		Singleton(PointerType instance) { s_Instance = instance; }
		virtual ~Singleton() { s_Instance = nullptr; }

	private:
		static inline PointerType s_Instance = nullptr;
	};
}

// GLM standards configuration
#include "Quark/Math/Types.h"

#include "Assertions.h"
#include "LogUtils.h"
#include "Timer.h"
#include "Timestep.h"

#include "Quark/Debug/Logger.h"
#include "Quark/Debug/Profiling.h"
#include "Quark/Debug/MemoryMetrics.h"
